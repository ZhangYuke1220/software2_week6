#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
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
static void save_symbol(Node *np, int *tmp);
static void display_tree(Node *np);
int find_symbol(int *tmp, int x, int n);
int count_deepth(Node *np);
int count_nodes(Node *np, int num);

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
/*        if (np->symbol == (int)'\n')
            printf("EOL %s\n", np->huffman_code);
        else if (np->symbol == (int)' ')
            printf("Space %s\n", np->huffman_code);
        else
            printf("%c %s\n", (char)np->symbol, np->huffman_code);*/
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

static void save_symbol(Node *np, int *tmp)
{
    static unsigned int a = 0;
    if (!np)
        return;
    else
    {
        save_symbol(np->left,tmp);
        tmp[a++] = np->symbol;
        save_symbol(np->right, tmp);
    }
}

int count_deepth(Node *np)
{
    if (!np)
        return 0;
    int left = count_deepth(np->left);
    int right = count_deepth(np->right);
    return (left >= right) ? left+1 : right+1;
}

int find_symbol(int *tmp, int x, int n)
{
    for (int i=0; i<n; ++i)
        if (tmp[i] == x)
            return i+1;
    return -1;
}

int count_nodes(Node *np, int num)
{
    if(!np)
        return num;
    else
    {
        num = count_nodes(np->left, num);
        num = count_nodes(np->right, num);
    }
    num++;
    return num;
}

static void display_tree(Node *np)
{
    const int max_digits = 2;
    const int dummy_symbol = -1;
    const int nodes_num = count_nodes(np, 0);
    printf("%d\n", nodes_num);
    int abs_distance[2];
    int symbol_array[nodes_num];
    int vert_line[nodes_num];
    int rlt_distance = 0;
    int left_line_num = 0;
    int right_line_num = 0;
    int right_line_num2 = 0;
    int vert_line_num = 0;
    int deepth = count_deepth(np);
    printf("%d\n", deepth);
    save_symbol(np, symbol_array);
    memset(symbol_array, 0, nodes_num);
    memset(vert_line, 0, nodes_num);

    Node *dummy_node = (Node *)malloc(sizeof(Node));
    *dummy_node = (Node){.right = NULL, .left = NULL, .symbol = dummy_symbol, .huffman_code = NULL};

    Node *tmp_arr[np->count];
    tmp_arr[0] = np;

    unsigned int k=0, j=1, n=1;

    while (n != deepth+1)
    {
        abs_distance[0] = 0;
        abs_distance[1] = 0;
        if (tmp_arr[k]->left != NULL && tmp_arr[k]->left != dummy_node)
            tmp_arr[j++] = tmp_arr[k]->left;
        else
            tmp_arr[j++] = dummy_node;
        if (tmp_arr[k]->right != NULL && tmp_arr[k]->right != dummy_node)
            tmp_arr[j++] = tmp_arr[k]->right;
        else
            tmp_arr[j++] = dummy_node;
        k++;
        if (k == pow(2, n)-1)
        {
            right_line_num2 = 0;
            vert_line_num = 0;
            for (int i = pow(2, n-1)-1; i<k; i++)
            {
                left_line_num = 0;
                right_line_num = 0;

                abs_distance[0] = find_symbol(symbol_array, tmp_arr[i]->symbol, nodes_num) * max_digits;
                //printf("%d %d\n", abs_distance[0], abs_distance[1]);
                if (abs_distance[0] < 0)
                    continue;
                rlt_distance = abs_distance[0] - abs_distance[1];

                abs_distance[0] += strlen(tmp_arr[i]->huffman_code) + 2;
                abs_distance[1] = abs_distance[0];
                //printf("%d %d\n", abs_distance[0], abs_distance[1]);

                if (tmp_arr[i]->left != dummy_node && tmp_arr[i]->left != NULL)
                {
                    left_line_num = (abs_distance[0] - find_symbol(symbol_array, tmp_arr[i]->left->symbol, nodes_num)) * max_digits
                                     - strlen(tmp_arr[i]->huffman_code) - 4;
                    vert_line[vert_line_num++] = find_symbol(symbol_array, tmp_arr[i]->left->symbol, nodes_num) * max_digits + 2;
                }
                if (tmp_arr[i]->right != dummy_node && tmp_arr[i]->right != NULL)
                {
                    right_line_num = find_symbol(symbol_array, tmp_arr[i]->right->symbol, nodes_num) * max_digits
                                     - abs_distance[0] + 2;
                    vert_line[vert_line_num++] = find_symbol(symbol_array, tmp_arr[i]->right->symbol, nodes_num) * max_digits + 1;
                }

                for (int m=0; m<(rlt_distance - left_line_num - right_line_num2); ++m)
                    printf(" ");
                for (int m=0; m<left_line_num; ++m)
                    printf("_");

                if (tmp_arr[i]->symbol != dummy_symbol)
                {
                    if (tmp_arr[i]->symbol == (int)(' '))
                        printf("Space %s", tmp_arr[i]->huffman_code);
                    else if (tmp_arr[i]->symbol == (int)('\n'))
                        printf("EOL %s", tmp_arr[i]->huffman_code);
                    else
                        printf("%c %s", (char)tmp_arr[i]->symbol, tmp_arr[i]->huffman_code);
                }

                for (int m=0; m<right_line_num; ++m)
                    printf("_");
                //printf("%d %d %d %d\n", left_line_num, right_line_num, right_line_num2, vert_line_num);

                right_line_num2 = right_line_num;
            }
            printf("\n");
            if (vert_line_num)
                for (int m=0; m<vert_line_num; ++m)
                {
                    for (int i=0; i<(m==0 ? vert_line[m] : (vert_line[m] - vert_line[m-1])); ++i)
                        printf(" ");
                    printf("|");
                    vert_line[m]++;
                }
            printf("\n");
            n++;
        }
    }
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
    display_tree(root);

    return EXIT_SUCCESS;
}
