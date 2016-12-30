#ifndef NODE_H_
#define NODE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYSTYPE pnode

enum node_type {A, B, C}; // A is nonterminal symbol, B is terminal symbol, C is terminal symbol with extra content
typedef struct Node {
    int rule;               // 规则序号
    char name[32];          // 名字
    int line;               // 行号
    enum node_type type;
    union {
        int type_int;       // 整形值
        float type_float;   // 浮点型
        char type_id[32];   // ID(relop)内容
    };
    struct Node *l;
    struct Node *r;
} node, *pnode;

void display_line(pnode ptr, int indent); 
        
void display(pnode root, int indent);
/*
pnode const_A(char *name, int line) {
    pnode ptr = (pnode)malloc(sizeof(node));
    ptr->line = line;
    strcpy(ptr->name, name);
    ptr->type = A;
    ptr->child = NULL;
    return ptr;
}
*/
#endif
