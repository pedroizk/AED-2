#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256
#define CODE_SZ 256

typedef struct n {
    int val;
    int cnt;
    struct n *l, *r;
} Node;

typedef struct {
    Node **arr;
    int sz;
    int cap;
} PQ;

typedef struct {
    char **tbl;
} Codes;

PQ* mk_pq(int cap) {
    PQ *p = malloc(sizeof(PQ));
    p->arr = malloc(sizeof(Node*) * cap);
    p->sz = 0;
    p->cap = cap;
    return p;
}

void add(PQ *p, Node *n) {
    int i = p->sz;
    p->sz++;
    while (i > 0 && p->arr[(i-1)/2]->cnt > n->cnt) {
        p->arr[i] = p->arr[(i-1)/2];
        i = (i-1)/2;
    }
    p->arr[i] = n;
}

Node* pop(PQ *p) {
    Node *m = p->arr[0];
    p->sz--;
    Node *last = p->arr[p->sz];

    int i = 0;
    while (2*i + 1 < p->sz) {
        int j = 2*i + 1;
        if (j + 1 < p->sz && p->arr[j + 1]->cnt < p->arr[j]->cnt)
            j++;
        if (last->cnt <= p->arr[j]->cnt)
            break;
        p->arr[i] = p->arr[j];
        i = j;
    }
    if (p->sz > 0)
        p->arr[i] = last;
    return m;
}

void free_pq(PQ *p) {
    free(p->arr);
    free(p);
}

Node* mk_node(int v, int c, Node *l, Node *r) {
    Node *n = malloc(sizeof(Node));
    n->val = v;
    n->cnt = c;
    n->l = l;
    n->r = r;
    return n;
}

void free_tree(Node *n) {
    if (n == NULL) return;
    free_tree(n->l);
    free_tree(n->r);
    free(n);
}

int is_leaf(Node *n) {
    return n->l == NULL && n->r == NULL;
}

void count(FILE *f, int *freq) {
    for (int i = 0; i < MAX; i++)
        freq[i] = 0;
    int c;
    while ((c = fgetc(f)) != EOF)
        freq[c]++;
}

Node* build(int *freq) {
    PQ *p = mk_pq(MAX);

    for (int i = 0; i < MAX; i++) {
        if (freq[i] > 0) {
            Node *n = mk_node(i, freq[i], NULL, NULL);
            add(p, n);
        }
    }

    if (p->sz == 0) {
        free_pq(p);
        return NULL;
    }

    while (p->sz > 1) {
        Node *a = pop(p);
        Node *b = pop(p);
        Node *parent = mk_node('\0', a->cnt + b->cnt, a, b);
        add(p, parent);
    }

    Node *root = pop(p);
    free_pq(p);
    return root;
}

void gen(Node *n, char *s, int d, char **tbl) {
    if (is_leaf(n)) {
        s[d] = '\0';
        tbl[n->val] = malloc(d + 1);
        strcpy(tbl[n->val], s);
        return;
    }
    if (n->l) {
        s[d] = '0';
        gen(n->l, s, d + 1, tbl);
    }
    if (n->r) {
        s[d] = '1';
        gen(n->r, s, d + 1, tbl);
    }
}

Codes* mk_codes(Node *root) {
    Codes *c = malloc(sizeof(Codes));
    c->tbl = malloc(sizeof(char*) * MAX);
    for (int i = 0; i < MAX; i++)
        c->tbl[i] = NULL;

    if (root == NULL) return c;

    char buf[CODE_SZ];
    if (is_leaf(root)) {
        c->tbl[root->val] = malloc(2);
        strcpy(c->tbl[root->val], "0");
    } else {
        gen(root, buf, 0, c->tbl);
    }
    return c;
}

void free_codes(Codes *c) {
    for (int i = 0; i < MAX; i++)
        if (c->tbl[i])
            free(c->tbl[i]);
    free(c->tbl);
    free(c);
}

void write_tree(Node *n, FILE *out, int *b, int *pos) {
    if (is_leaf(n)) {
        if (*pos == 8) {
            fputc(*b, out);
            *b = 0;
            *pos = 0;
        }
        *b |= (1 << (7 - *pos));
        (*pos)++;

        for (int i = 7; i >= 0; i--) {
            if (*pos == 8) {
                fputc(*b, out);
                *b = 0;
                *pos = 0;
            }
            if (n->val & (1 << i))
                *b |= (1 << (7 - *pos));
            (*pos)++;
        }
    } else {
        if (*pos == 8) {
            fputc(*b, out);
            *b = 0;
            *pos = 0;
        }
        (*pos)++;
        write_tree(n->l, out, b, pos);
        write_tree(n->r, out, b, pos);
    }
}

void compress(const char *in, const char *out) {
    FILE *fin = fopen(in, "rb");

    int freq[MAX];
    count(fin, freq);

    Node *root = build(freq);
    Codes *codes = mk_codes(root);

    fseek(fin, 0, SEEK_END);
    long sz = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    FILE *fout = fopen(out, "wb");

    fwrite(&sz, sizeof(long), 1, fout);

    int b = 0;
    int pos = 0;
    write_tree(root, fout, &b, &pos);

    int c;
    while ((c = fgetc(fin)) != EOF) {
        char *code = codes->tbl[c];
        for (int i = 0; code[i]; i++) {
            if (pos == 8) {
                fputc(b, fout);
                b = 0;
                pos = 0;
            }
            if (code[i] == '1')
                b |= (1 << (7 - pos));
            pos++;
        }
    }

    if (pos > 0)
        fputc(b, fout);

    fclose(fin);
    fclose(fout);
    free_codes(codes);
    free_tree(root);

    printf("ok\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("uso: %s <entrada> <saida.huf>\n", argv[0]);
        return 1;
    }

    compress(argv[1], argv[2]);
    return 0;
}
