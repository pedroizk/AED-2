#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PALAVRA 256
#define NUM_DIGITOS 8

typedef struct Palavra {
    char texto[MAX_PALAVRA];
    struct Palavra *prox;
} Palavra;

typedef struct No {
    struct No *filhos[NUM_DIGITOS];
    Palavra *palavras;
} No;

No* criar_no() {
    No *no = (No*)malloc(sizeof(No));

    for(int i = 0; i < NUM_DIGITOS; i++) {
        no->filhos[i] = NULL;
    }
    no->palavras = NULL;

    return no;
}

int char_para_digito(char c) {
    c = toupper(c);
    if(c >= 'A' && c <= 'C') return 2;
    if(c >= 'D' && c <= 'F') return 3;
    if(c >= 'G' && c <= 'I') return 4;
    if(c >= 'J' && c <= 'L') return 5;
    if(c >= 'M' && c <= 'O') return 6;
    if(c >= 'P' && c <= 'S') return 7;
    if(c >= 'T' && c <= 'V') return 8;
    if(c >= 'W' && c <= 'Z') return 9;
    return -1;
}

void adicionar_palavra(No *no, char *texto) {
    Palavra *nova = (Palavra*)malloc(sizeof(Palavra));
    strcpy(nova->texto, texto);
    nova->prox = no->palavras;
    no->palavras = nova;
}

void inserir(No *raiz, char *palavra) {
    No *atual = raiz;

    for(int i = 0; palavra[i] != '\0'; i++) {
        int digito = char_para_digito(palavra[i]);
        if(digito == -1) continue;

        int indice = digito - 2;

        if(atual->filhos[indice] == NULL) {
            atual->filhos[indice] = criar_no();
        }
        atual = atual->filhos[indice];
    }

    adicionar_palavra(atual, palavra);
}

void imprimir_palavras(Palavra *lista) {
    while(lista != NULL) {
        printf("%s\n", lista->texto);
        lista = lista->prox;
    }
}

void buscar_e_imprimir(No *no) {
    if(no == NULL) return;

    if(no->palavras != NULL) {
        imprimir_palavras(no->palavras);
    }

    for(int i = 0; i < NUM_DIGITOS; i++) {
        buscar_e_imprimir(no->filhos[i]);
    }
}

int buscar(No *raiz, char *digitos, int exata) {
    No *atual = raiz;

    for(int i = 0; digitos[i] != '\0'; i++) {
        int digito = digitos[i] - '0';
        if(digito < 2 || digito > 9) return 0;

        int indice = digito - 2;

        if(atual->filhos[indice] == NULL) return 0;
        atual = atual->filhos[indice];
    }

    if(exata) {
        if(atual->palavras != NULL) {
            imprimir_palavras(atual->palavras);
            return 1;
        }
        return 0;
    }

    buscar_e_imprimir(atual);
    return 1;
}

int carregar_dicionario(No *raiz, char *arquivo) {
    FILE *f = fopen(arquivo, "r");
    if(f == NULL) return 0;

    char palavra[MAX_PALAVRA];
    int total = 0;

    while(fgets(palavra, MAX_PALAVRA, f) != NULL) {
        palavra[strcspn(palavra, "\n")] = '\0';
        if(strlen(palavra) > 0) {
            inserir(raiz, palavra);
            total++;
        }
    }

    fclose(f);
    return total;
}

int main(int argc, char *argv[]) {
    char *arquivo = "palavras.txt";
    if(argc > 1) arquivo = argv[1];

    No *raiz = criar_no();
    int total = carregar_dicionario(raiz, arquivo);

    printf("=== T9 ===\n");
    printf("Carregado: %d palavras\n\n", total);
    printf("2→ABC 3→DEF 4→GHI 5→JKL 6→MNO 7→PQRS 8→TUV 9→WXYZ\n");
    printf("Use # para busca exata. Digite 0 para sair.\n\n");

    char entrada[MAX_PALAVRA];

    while(1) {
        printf("> ");
        if(fgets(entrada, MAX_PALAVRA, stdin) == NULL) break;

        entrada[strcspn(entrada, "\n")] = '\0';

        if(strcmp(entrada, "0") == 0 || strcmp(entrada, "sair") == 0 || strlen(entrada) == 0) break;

        int exata = 0;
        char *seq = entrada;

        if(entrada[0] == '#') {
            exata = 1;
            seq = entrada + 1;
        }

        if(!buscar(raiz, seq, exata)) {
            printf("Nenhuma palavra encontrada.\n");
        }
        printf("\n");
    }

    return 0;
}
