// cidades são os vértices e as distâncias são as arestas
// Implementação usando Lista de Adjacências
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct coordenada {
    double x;
    double y;
} coordenada;

typedef struct aresta {
    int destino;
    long long distancia;
    struct aresta* proxima;
} Aresta;

typedef struct vertice {
    int id;
    Aresta* adjacentes;
} Vertice;

typedef struct grafoLista {
    Vertice* vertices;
    coordenada* coords;
    int tamanho;
    char nome[20];
} grafo;

long long calcularDistancia(coordenada c1, coordenada c2) {
    double xd = c1.x - c2.x;
    double yd = c1.y - c2.y;
    return (long long)(sqrt(xd*xd + yd*yd) + 0.5);
}

// COMPLEXIDADE: O(n^2) tempo para grafo completo, O(n) espaço para leitura
grafo* construtorLista(char* fileName) {
    grafo* g;
    FILE* pTsp;
    char auxString[100];
    char targetChar = ':';
    char* pPosition;
    char* afterString;
    int i, j;

    g = (grafo*)malloc(sizeof(grafo));
    pTsp = fopen(fileName, "r");

    if(!pTsp) {
        printf("Erro ao abrir arquivo\n");
        free(g);
        return NULL;
    }

    fgets(auxString, 100, pTsp);
    pPosition = strchr(auxString, targetChar);
    afterString = pPosition + 1;
    strcpy(g->nome, afterString + 1);

    while (fgets(auxString, 100, pTsp) && strstr(auxString, "COMMENT") != NULL);
    fgets(auxString, 100, pTsp);

    pPosition = strchr(auxString, ':');
    g->tamanho = atoi(pPosition + 1);

    fgets(auxString, 100, pTsp);
    fgets(auxString, 100, pTsp);

    g->vertices = (Vertice*)malloc(sizeof(Vertice) * g->tamanho);
    g->coords = (coordenada*)malloc(sizeof(coordenada) * g->tamanho);

    for(i = 0; i < g->tamanho; i++) {
        int id;
        fscanf(pTsp, "%d %lf %lf", &id, &g->coords[i].x, &g->coords[i].y);
        g->vertices[i].id = i;
        g->vertices[i].adjacentes = NULL;
    }

    for(i = 0; i < g->tamanho; i++) {
        for(j = 0; j < g->tamanho; j++) {
            if(i != j) {
                Aresta* nova = (Aresta*)malloc(sizeof(Aresta));
                nova->destino = j;
                nova->distancia = calcularDistancia(g->coords[i], g->coords[j]);
                nova->proxima = g->vertices[i].adjacentes;
                g->vertices[i].adjacentes = nova;
            }
        }
    }

    fclose(pTsp);
    return g;
}

void imprimirLista(grafo* g, int limite) {
    int i, count;
    Aresta* atual;

    printf("Grafo: %s", g->nome);
    printf("Tamanho: %d vertices\n", g->tamanho);
    printf("Lista de Adjacencias (primeiros %d vertices):\n\n", limite);

    for(i = 0; i < g->tamanho && i < limite; i++) {
        printf("Vertice %d: ", i);
        atual = g->vertices[i].adjacentes;
        count = 0;
        while(atual != NULL && count < 5) {
            printf("(%d, dist=%lld) ", atual->destino, atual->distancia);
            atual = atual->proxima;
            count++;
        }
        if(atual != NULL) {
            printf("...");
        }
        printf("\n");
    }
}

// COMPLEXIDADE: O(n) tempo, O(n) espaço adicional
grafo* inserirVertice(grafo* g, double x, double y) {
    int novoTamanho, i;
    Vertice* novosVertices;
    coordenada* novasCoords;
    Aresta* nova;

    novoTamanho = g->tamanho + 1;

    novosVertices = (Vertice*)malloc(sizeof(Vertice) * novoTamanho);
    novasCoords = (coordenada*)malloc(sizeof(coordenada) * novoTamanho);

    for(i = 0; i < g->tamanho; i++) {
        novosVertices[i] = g->vertices[i];
        novasCoords[i] = g->coords[i];
    }

    novosVertices[g->tamanho].id = g->tamanho;
    novosVertices[g->tamanho].adjacentes = NULL;
    novasCoords[g->tamanho].x = x;
    novasCoords[g->tamanho].y = y;

    for(i = 0; i < g->tamanho; i++) {
        nova = (Aresta*)malloc(sizeof(Aresta));
        nova->destino = g->tamanho;
        nova->distancia = calcularDistancia(novasCoords[i], novasCoords[g->tamanho]);
        nova->proxima = novosVertices[i].adjacentes;
        novosVertices[i].adjacentes = nova;
    }

    for(i = 0; i < g->tamanho; i++) {
        nova = (Aresta*)malloc(sizeof(Aresta));
        nova->destino = i;
        nova->distancia = calcularDistancia(novasCoords[g->tamanho], novasCoords[i]);
        nova->proxima = novosVertices[g->tamanho].adjacentes;
        novosVertices[g->tamanho].adjacentes = nova;
    }

    free(g->vertices);
    free(g->coords);

    g->vertices = novosVertices;
    g->coords = novasCoords;
    g->tamanho = novoTamanho;

    return g;
}

// COMPLEXIDADE: O(n * m) tempo onde m = grau médio, O(1) espaço adicional
grafo* removerVertice(grafo* g, int vertice) {
    int novoTamanho, ni, i;
    Vertice* novosVertices;
    coordenada* novasCoords;
    Aresta *atual, *anterior, *temp;

    if(vertice < 0 || vertice >= g->tamanho) {
        printf("Vertice invalido\n");
        return g;
    }

    atual = g->vertices[vertice].adjacentes;
    while(atual != NULL) {
        temp = atual;
        atual = atual->proxima;
        free(temp);
    }

    for(i = 0; i < g->tamanho; i++) {
        if(i == vertice) continue;

        anterior = NULL;
        atual = g->vertices[i].adjacentes;

        while(atual != NULL) {
            if(atual->destino == vertice) {
                if(anterior == NULL) {
                    g->vertices[i].adjacentes = atual->proxima;
                } else {
                    anterior->proxima = atual->proxima;
                }
                temp = atual;
                atual = atual->proxima;
                free(temp);
            } else {
                if(atual->destino > vertice) {
                    atual->destino--;
                }
                anterior = atual;
                atual = atual->proxima;
            }
        }
    }

    novoTamanho = g->tamanho - 1;
    novosVertices = (Vertice*)malloc(sizeof(Vertice) * novoTamanho);
    novasCoords = (coordenada*)malloc(sizeof(coordenada) * novoTamanho);

    ni = 0;
    for(i = 0; i < g->tamanho; i++) {
        if(i == vertice) continue;
        novosVertices[ni] = g->vertices[i];
        novosVertices[ni].id = ni;
        novasCoords[ni] = g->coords[i];
        ni++;
    }

    free(g->vertices);
    free(g->coords);

    g->vertices = novosVertices;
    g->coords = novasCoords;
    g->tamanho = novoTamanho;

    return g;
}

// COMPLEXIDADE: O(m) tempo onde m = grau do vértice, O(1) espaço
bool buscarAresta(grafo* g, int origem, int destino) {
    Aresta* atual;

    if(origem < 0 || origem >= g->tamanho || destino < 0 || destino >= g->tamanho) {
        return false;
    }

    atual = g->vertices[origem].adjacentes;
    while(atual != NULL) {
        if(atual->destino == destino) {
            return true;
        }
        atual = atual->proxima;
    }

    return false;
}

// COMPLEXIDADE: O(m) tempo onde m = grau do vértice, O(1) espaço
void editarAresta(grafo* g, int origem, int destino, long long novaDistancia) {
    Aresta* atual;

    if(origem < 0 || origem >= g->tamanho || destino < 0 || destino >= g->tamanho) {
        printf("Vertices invalidos\n");
        return;
    }

    atual = g->vertices[origem].adjacentes;
    while(atual != NULL) {
        if(atual->destino == destino) {
            atual->distancia = novaDistancia;
            break;
        }
        atual = atual->proxima;
    }

    atual = g->vertices[destino].adjacentes;
    while(atual != NULL) {
        if(atual->destino == origem) {
            atual->distancia = novaDistancia;
            break;
        }
        atual = atual->proxima;
    }
}

void destruirGrafo(grafo* g) {
    int i;
    Aresta *atual, *temp;

    if(g) {
        if(g->vertices) {
            for(i = 0; i < g->tamanho; i++) {
                atual = g->vertices[i].adjacentes;
                while(atual != NULL) {
                    temp = atual;
                    atual = atual->proxima;
                    free(temp);
                }
            }
            free(g->vertices);
        }
        if(g->coords) {
            free(g->coords);
        }
        free(g);
    }
}

void imprimirAnaliseComplexidade() {
    printf("\nComplexidade Lista:\n");
    printf("Inserir vertice: O(n)\n");
    printf("Remover vertice: O(n*m)\n");
    printf("Buscar aresta: O(m)\n");
    printf("Editar aresta: O(m)\n");
    printf("Espaco: O(n+m)\n");
}

int main() {
    grafo* g;
    Aresta* a;

    printf("teste lista\n");

    g = construtorLista("wi29.tsp");

    if(!g) {
        printf("erro\n");
        return 1;
    }

    printf("vertices: %d\n", g->tamanho);

    printf("\nbusca aresta 0-1: ");
    if(buscarAresta(g, 0, 1)) {
        a = g->vertices[0].adjacentes;
        while(a && a->destino != 1) a = a->proxima;
        printf("existe, dist=%lld\n", a ? a->distancia : -1);
    }

    printf("\nedicao aresta 0-1:\n");
    a = g->vertices[0].adjacentes;
    while(a && a->destino != 1) a = a->proxima;
    printf("antes: %lld\n", a ? a->distancia : -1);
    editarAresta(g, 0, 1, 999);
    a = g->vertices[0].adjacentes;
    while(a && a->destino != 1) a = a->proxima;
    printf("depois: %lld\n", a ? a->distancia : -1);

    printf("\ninserir vertice:\n");
    printf("antes: %d vertices\n", g->tamanho);
    inserirVertice(g, 30000.0, 15000.0);
    printf("depois: %d vertices\n", g->tamanho);

    printf("\nremover vertice 28:\n");
    printf("antes: %d vertices\n", g->tamanho);
    removerVertice(g, 28);
    printf("depois: %d vertices\n", g->tamanho);

    imprimirAnaliseComplexidade();

    destruirGrafo(g);

    return 0;
}
