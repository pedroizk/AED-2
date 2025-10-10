#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
#define N_ALFABETO 256

typedef int bool;
typedef bool TIPORET;


typedef struct no {
    struct no *filhos[N_ALFABETO];
    char* tipo;
    int polaridade;
    TIPORET fim;
} nodo;

typedef nodo* ponto;

ponto criaNodo() {
    ponto p = NULL;

    p = (ponto)malloc(sizeof(nodo));

    if(p) {
        p->fim = false;
        p->tipo = NULL;
        p->polaridade = 0;
        for(int i = 0; i < N_ALFABETO; i++) {
            p->filhos[i] = NULL;
        }
    }
    return p;
}

ponto inicializa() {
    return(criaNodo());
}

int mapearIndice(char c) {
    int indice = (int)c;
    if(indice < 0 || indice >= N_ALFABETO) {
        return -1;
    }
    return indice;
}

void insere(ponto raiz, char* chave, char* tipo, int polaridade) {
    int nivel;
    int comprimento = strlen(chave);
    int i;

    if(!raiz || !chave) return;

    ponto p = raiz;
    for(nivel = 0; nivel < comprimento; nivel++) {
        i = mapearIndice(chave[nivel]);
        if(i == -1) return;
        if(!p->filhos[i]) {
            p->filhos[i] = criaNodo();
        }

        p = p->filhos[i];
    }
    p->fim = true;

    if(tipo != NULL) {
        p->tipo = (char*)malloc((strlen(tipo)) + 1);
        if(p->tipo) {
            strcpy(p->tipo, tipo);
        }
    }
    p->polaridade = polaridade;
}

ponto busca(ponto raiz, char * chave) {
    int nivel;
    int comprimento = strlen(chave);
    int i;

    if(!raiz || !chave) return NULL;

    ponto p = raiz;

    for(nivel = 0; nivel < comprimento; nivel++) {
        i = mapearIndice(chave[nivel]);
        if(i == -1 || !p->filhos[i])
            return NULL;
        p = p->filhos[i];
    }

    return (p->fim) ? p : NULL;
}


void criaTrieUsandoCsv(ponto raiz) {
    FILE *fptr; 
    fptr = fopen("OpLexicon.csv", "r");
    char linha[256];
    char* palavra;
    char* tipo;
    int polaridade;

    if(fptr == NULL) {
        printf("Erro ao abrir arquivo");
        return;
    }

    while(fgets(linha, sizeof(linha), fptr) != NULL) {
        palavra = strtok(linha, ",");
        tipo = strtok(NULL, ",");
        char* pol_str = strtok(NULL, ",");
        if(pol_str != NULL) {
            polaridade = atoi(pol_str);
        }

        if(palavra != NULL) {
            insere(raiz, palavra, tipo, polaridade);
        }
    }

    fclose(fptr);
}

void salvarArquivo_aux(ponto p, FILE* arquivo, char* palavra, int nivel) {
    if(p == NULL) return;

    if(p->fim) {
        palavra[nivel] = '\0';
        fprintf(arquivo, "%s,%s,%d,A\n", palavra, p->tipo ? p->tipo : "", p->polaridade);
    }

    for(int i = 0; i < N_ALFABETO; i++) {
        if(p->filhos[i]) {
            palavra[nivel] = (char)i;
            salvarArquivo_aux(p->filhos[i], arquivo, palavra, nivel + 1);
        }
    }
}

void salvarArquivo(ponto raiz) {
    FILE* arquivo = fopen("lexico_atualizado.txt", "w");
    if(!arquivo) {
        printf("Erro ao criar arquivo!\n");
        return;
    }

    char palavra[256];
    salvarArquivo_aux(raiz, arquivo, palavra, 0);
    fclose(arquivo);
    printf("Arquivo salvo como 'lexico_atualizado.txt'\n");
}

int main(void) {
    ponto inicial = inicializa();
    
    int loop = 0;
    int escolha;
    char palavra[256];

    criaTrieUsandoCsv(inicial);
    
    while(loop != 1) {
        printf("\nEscolha uma das opções: \n 1. Busca de polaridade \n 2. Edição de polaridade \n 3. Salvar arquivo \n 4. Sair \n Escolha: ");
        scanf("%d", &escolha);

        switch(escolha) {
            case(1):
                printf("\n Digite a palavra a ser buscada: ");
                scanf("%s", palavra);
                ponto nodo_encontrado = busca(inicial, palavra);
                if(nodo_encontrado) {
                    printf("Palavra %s encontrada!\n", palavra);
                    printf("Tipo: %s\n", nodo_encontrado->tipo ? nodo_encontrado->tipo : "");
                    printf("Polaridade: %d\n", nodo_encontrado->polaridade);
                } else {
                    printf("Palavra não encontrada!\n");
                }
                break;
            case(2):
                printf("\n Digite a palavra para editar polaridade: ");
                scanf("%s", palavra);
                ponto nodo_encontrado2 = busca(inicial, palavra);
                if(nodo_encontrado2) {
                    printf("Palavra encontrada! Polaridade atual: %d\n", nodo_encontrado2->polaridade);
                    printf("Digite a nova polaridade: ");
                    int nova_polaridade;
                    scanf("%d", &nova_polaridade);
                    nodo_encontrado2->polaridade = nova_polaridade;
                    printf("Polaridade atualizada!\n");
                } else {
                    printf("Palavra não encontrada!\n");
                }
                break;
            case(3):
                salvarArquivo(inicial);
                break;
            case(4):
                printf("Saindo...\n");
                loop = 1;
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    }
}

