#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "huffman_encode.h"

static const int nsymbols = 256 + 1;
int *symbol_count;

typedef struct node
{
    int symbol;
    int count;
    char *huffman_code;
    struct node *left;
    struct node *right;
} Node;


static void count_symbols(const char *filename);
static Node *create_node(int symbol, int count, Node *left, Node *right);
static Node *pop_min(int *n, Node *nodep[]);
static Node *build_tree(void);
static void traverse_tree(const int depth, const Node *np);

static void count_symbols(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "error: cannot open %s\n", filename);
        exit(1);
    }

    symbol_count = (int *)calloc(nsymbols, sizeof(int));
    for (int i=0; i<nsymbols; ++i)
        symbol_count[i] = 0;

    char buf;
    while (fread(&buf, sizeof(char), 1, fp))
        symbol_count[buf - '\0'] += 1;

    for (int i=0; i<nsymbols; ++i)
        if (symbol_count[i] != 0)
            printf("%c, %d\n", (char)i, symbol_count[i]);

    fclose(fp);
}

static Node *create_node(int symbol, int count, Node *left, Node *right)
{
    Node *ret = (Node *)malloc(sizeof(Node));
    *ret = (Node){.symbol = symbol, .count = count, .left = left, .right = right};
    return ret;
}

static Node *pop_min(int *n, Node *nodep[])
{
    int argmin = 0;
    for (int i = 0; i < *n; i++)
        if (nodep[i]->count < nodep[argmin]->count)
            argmin = i;

    Node *node_min = nodep[argmin];

    for (int i = argmin; i < (*n) - 1; i++)
        nodep[i] = nodep[i + 1];

    (*n)--;

    return node_min;
}

static Node *build_tree()
{
    int n = 0;
    Node *nodep[nsymbols];

    for (int i = 0; i < nsymbols; i++)
    {
        if (symbol_count[i] == 0)
            continue;

        nodep[n] = create_node(i, symbol_count[i], NULL, NULL);
        n++;
    }

    const int dummy = -1;
    while (n >= 2)
    {
        Node *node1 = pop_min(&n, nodep);
        Node *node2 = pop_min(&n, nodep);

        nodep[n] = create_node(dummy, node1->count + node2->count, node1, node2);
        n++;
    }

    free(symbol_count);
    nodep[0]->huffman_code = "\0";
    return (n == 0) ? NULL : nodep[0];
}

static void traverse_tree(const int depth, const Node *np)
{
    
    if (np->left == NULL && np->right == NULL)
    {
        if (np->symbol == (int)'\n')
            printf("EOL %s\n", np->huffman_code);
        else if (np->symbol == (int)' ')
            printf("Space %s\n", np->huffman_code);
        else
            printf("%c %s\n", (char)np->symbol, np->huffman_code);
        return;
    }

    if (np->left != NULL)
    {
        np->left->huffman_code = (char *)malloc(sizeof(char) * (strlen(np->huffman_code) + 1));
        strcpy(np->left->huffman_code, np->huffman_code);
        np->left->huffman_code[strlen(np->huffman_code)] = '0';
    }

    if (np->right != NULL)
    {
        np->right->huffman_code = (char *)malloc(sizeof(char) * (strlen(np->huffman_code) + 1));
        strcpy(np->right->huffman_code, np->huffman_code);
        np->right->huffman_code[strlen(np->huffman_code)] = '1';
    }
    traverse_tree(depth + 1, np->left);
    traverse_tree(depth + 1, np->right);
}

int encode(const char *filename)
{
    count_symbols(filename);
    Node *root = build_tree();

    if (root == NULL)
    {
        fprintf(stderr, "A tree has not been constructed.\n");
        return EXIT_FAILURE;
    }

    traverse_tree(0, root);
    return EXIT_SUCCESS;
}
