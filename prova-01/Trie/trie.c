// Objetivo -> criar sem consultas todas as funções fundamentais de uma TRIE
// Inserção, busca e exclusão para o alfabeto arábico
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LENGTH 26

typedef struct TrieNode {
    struct TrieNode* child[MAX_LENGTH];
    bool isEnd;
} no;

// getIndex
int getIndex(char letter) {
    return ((char)letter - (char)'a');
}

no* createNode() {
    no* newNode = NULL;
    newNode = (no*)malloc(sizeof(no));

    for(int i = 0; i < MAX_LENGTH; i++) {
        newNode->child[i] = NULL;
    }

    newNode->isEnd = false;

    return newNode;
}

no* createTrie() {
    return createNode();
}

// search 
bool search(char* word, no* root) {
    int wordLength = strlen(word);
    no* auxPointer = root;
    
    for(int i = 0; i < wordLength; i++) {
        char nowChar = word[i];
        int nextIndex = getIndex(nowChar);

        if(auxPointer->child[nextIndex] == NULL) {
            return false;
        }

        auxPointer = auxPointer->child[nextIndex];
    }

    if(auxPointer->isEnd == false)
        return false;

    return true;
}

void insert(char* word, no* root) {
    int wordLength = strlen(word);
    no* auxPointer = root;

    for(int i = 0; i < wordLength; i++) {
        char nowChar = word[i];
        int nextIndex = getIndex(nowChar);

        if(auxPointer->child[nextIndex] == NULL) {
            auxPointer->child[nextIndex] = createNode();
        }

        auxPointer = auxPointer->child[nextIndex];
    }

    auxPointer->isEnd = true;
}


int main(void) {

}