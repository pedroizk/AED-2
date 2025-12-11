// cidades são os vértices e as distâncias são as arestas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct cidade {
    long long distancia;
} Vertice;

typedef struct matriz {
    Vertice** matriz;
    char tipo[20];
    int tamanho;
    char nome[20];
} grafo;

typedef struct coordenada {
    double x;
    double y;
} coordenada;

grafo* construtorMatriz(char* fileName) {

    grafo* Grafo = (grafo*)malloc(sizeof(grafo));
    FILE* pTsp;
    pTsp = fopen(fileName, "r");

    if(!pTsp) {
        printf("Erro ao abrir arquivo\n");
        free(Grafo);
        return NULL;
    }

    char auxString[100];
    char targetChar = ':';
    fgets(auxString, 100, pTsp);

    char* pPosition = strchr(auxString, targetChar);
    char* afterString = pPosition + 1;
    strcpy(Grafo->nome, afterString + 1);

    while (fgets(auxString, 100, pTsp) && strstr(auxString, "COMMENT") != NULL);
    fgets(auxString, 100, pTsp);

    pPosition = strchr(auxString, ':');
    Grafo->tamanho = atoi(pPosition + 1);

    fgets(auxString, 100, pTsp);
    fgets(auxString, 100, pTsp);

    Grafo->matriz = (Vertice**)malloc(sizeof(Vertice*) * Grafo->tamanho);
    for(int k = 0; k < Grafo->tamanho; k++) {
        Grafo->matriz[k] = (Vertice*)malloc(sizeof(Vertice) * Grafo->tamanho);
    }

    coordenada* coords = (coordenada*)malloc(sizeof(coordenada) * Grafo->tamanho);

    for(int i = 0; i < Grafo->tamanho; i++) {
        int id;
        fscanf(pTsp, "%d %lf %lf", &id, &coords[i].x, &coords[i].y);
    }

    for(int i = 0; i < Grafo->tamanho; i++) {
        for(int j = 0; j < Grafo->tamanho; j++) {
            double xd = coords[i].x - coords[j].x;
            double yd = coords[i].y - coords[j].y;
            long long d = (long long)(sqrt(xd*xd + yd*yd) + 0.5);
            Grafo->matriz[i][j].distancia = d;
        }
    }

    free(coords);
    fclose(pTsp);
    return Grafo;
}

void imprimirMatriz(grafo* g) {
    printf("Grafo: %s", g->nome);
    printf("Tamanho: %d vertices\n", g->tamanho);
    printf("Matriz de Adjacencia (distancias):\n\n");

    for(int i = 0; i < g->tamanho; i++) {
        for(int j = 0; j < g->tamanho; j++) {
            printf("%4lld ", g->matriz[i][j].distancia);
        }
        printf("\n");
    }
}

// COMPLEXIDADE: O(n^2) tempo, O(n^2) espaço adicional
grafo* inserirVertice(grafo* g, double x, double y) {
    int novoTamanho = g->tamanho + 1;

    Vertice** novaMatriz = (Vertice**)malloc(sizeof(Vertice*) * novoTamanho);
    for(int k = 0; k < novoTamanho; k++) {
        novaMatriz[k] = (Vertice*)malloc(sizeof(Vertice) * novoTamanho);
    }

    for(int i = 0; i < g->tamanho; i++) {
        for(int j = 0; j < g->tamanho; j++) {
            novaMatriz[i][j].distancia = g->matriz[i][j].distancia;
        }
    }

    novaMatriz[g->tamanho][g->tamanho].distancia = 0;

    for(int k = 0; k < g->tamanho; k++) {
        free(g->matriz[k]);
    }
    free(g->matriz);

    g->matriz = novaMatriz;
    g->tamanho = novoTamanho;

    return g;
}

// COMPLEXIDADE: O(n^2) tempo, O(n^2) espaço adicional
grafo* removerVertice(grafo* g, int vertice) {
    int novoTamanho;
    Vertice** novaMatriz;
    int ni, nj, i, j, k;

    if(vertice < 0 || vertice >= g->tamanho) {
        printf("Vertice invalido\n");
        return g;
    }

    novoTamanho = g->tamanho - 1;

    if(novoTamanho == 0) {
        for(k = 0; k < g->tamanho; k++) {
            free(g->matriz[k]);
        }
        free(g->matriz);
        g->matriz = NULL;
        g->tamanho = 0;
        return g;
    }

    novaMatriz = (Vertice**)malloc(sizeof(Vertice*) * novoTamanho);
    for(k = 0; k < novoTamanho; k++) {
        novaMatriz[k] = (Vertice*)malloc(sizeof(Vertice) * novoTamanho);
    }

    ni = 0;
    for(i = 0; i < g->tamanho; i++) {
        if(i == vertice) continue;
        nj = 0;
        for(j = 0; j < g->tamanho; j++) {
            if(j == vertice) continue;
            novaMatriz[ni][nj].distancia = g->matriz[i][j].distancia;
            nj++;
        }
        ni++;
    }

    for(k = 0; k < g->tamanho; k++) {
        free(g->matriz[k]);
    }
    free(g->matriz);

    g->matriz = novaMatriz;
    g->tamanho = novoTamanho;

    return g;
}

// COMPLEXIDADE: O(1) tempo, O(1) espaço
bool buscarAresta(grafo* g, int origem, int destino) {
    if(origem < 0 || origem >= g->tamanho || destino < 0 || destino >= g->tamanho) {
        return false;
    }

    return g->matriz[origem][destino].distancia >= 0;
}

// COMPLEXIDADE: O(1) tempo, O(1) espaço
void editarAresta(grafo* g, int origem, int destino, long long novaDistancia) {
    if(origem < 0 || origem >= g->tamanho || destino < 0 || destino >= g->tamanho) {
        printf("Vertices invalidos\n");
        return;
    }

    g->matriz[origem][destino].distancia = novaDistancia;
    g->matriz[destino][origem].distancia = novaDistancia;
}

void destruirGrafo(grafo* g) {
    if(g) {
        if(g->matriz) {
            for(int k = 0; k < g->tamanho; k++) {
                free(g->matriz[k]);
            }
            free(g->matriz);
        }
        free(g);
    }
}

void imprimirAnaliseComplexidade() {
    printf("\nComplexidade Matriz:\n");
    printf("Inserir vertice: O(n^2)\n");
    printf("Remover vertice: O(n^2)\n");
    printf("Buscar aresta: O(1)\n");
    printf("Editar aresta: O(1)\n");
    printf("Espaco: O(n^2)\n");
}

int main() {

    printf("teste matriz\n");

    grafo* g = construtorMatriz("wi29.tsp");

    if(!g) {
        printf("erro\n");
        return 1;
    }

    printf("vertices: %d\n", g->tamanho);

    printf("\nbusca aresta 0-1: ");
    if(buscarAresta(g, 0, 1)) {
        printf("existe, dist=%lld\n", g->matriz[0][1].distancia);
    }

    printf("\nedicao aresta 0-1:\n");
    printf("antes: %lld\n", g->matriz[0][1].distancia);
    editarAresta(g, 0, 1, 999);
    printf("depois: %lld\n", g->matriz[0][1].distancia);

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