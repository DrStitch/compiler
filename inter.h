#ifndef INTER_H_
#define INTER_H_
#include "node.h"
enum op_kind { O_VARIABLE, O_TEMP, O_CONSTANT, O_LABEL, O_FUNC, O_RELOP,
   O_SIZE, O_ADDR, O_STAR};
enum code_kind { C_LABEL, C_FUNC, C_ASSIGN, C_ADD, C_SUB, C_MUL, C_DIV,
   C_GOTO, C_IFGT, C_RETURN, C_DEC, C_ARG, C_CALL, C_PARAM, C_READ, C_WRITE};

typedef struct Operand_ {
    enum op_kind kind;
    union {
        int var_no;
        int value;
        char *str;
    };
} Operand;

typedef struct InterCode_ {
    enum code_kind kind;
    Operand *x, *y, *z, *relop;
    struct InterCode_ *prev, *next;
} InterCode;

Operand * newTemp();
Operand * newLabel();
Operand * newOp(enum op_kind);
InterCode * newCode(enum code_kind);

void addCode(InterCode *);
void addLabel(Operand *);
void addGoto(Operand *);
InterCode * rmCode(InterCode *);

void translate(pnode);
Operand * translateVarDec(pnode);
void translateFunDec(pnode);
void translateVarList(pnode);
void translateCompSt(pnode);
void translateDefList(pnode);
void translateDecList(pnode);
void translateDec(pnode);
void translateStmtList(pnode);
Operand * translateExp(pnode);
void translateStmt(pnode);
void translateCond(pnode, Operand *, Operand *);
void translateArgs(pnode);

void optimize();

void displayOperand(Operand *);
void displayInterCode();

void generateInterCode(pnode root);

#endif
