#include "inter.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>

InterCode *code_head, *code_last;
int temp_no = 0;
int label_no = 0;

Operand * newTemp() {
    Operand * temp =  (Operand *)malloc(sizeof(Operand));
    temp->kind = O_TEMP;
    temp->str = (char *)malloc(8);
    sprintf(temp->str, "t%d", temp_no);
    temp_no++;
    return temp;
}

Operand * newLabel() {
    Operand * label = (Operand *)malloc(sizeof(Operand));
    label->kind = O_LABEL;
    label->var_no = label_no;
    label_no++;
    return label;
}

Operand * newOp(enum op_kind kind) {
    Operand * new = (Operand *)malloc(sizeof(Operand));
    new->kind = kind;
    new->value = 0;
    return new;
}

InterCode * newCode(enum code_kind kind) {
    InterCode * new = (InterCode *)malloc(sizeof(InterCode));
    new->kind = kind;
    new->x = NULL;
    new->y = NULL;
    new->z = NULL;
    new->relop = NULL;
    new->prev = NULL;
    new->next = NULL;
    return new;
}

void addCode(InterCode * code) {
    code_last->next = code;
    code->prev = code_last;
    code_last = code;
}

void addLabel(Operand * label) {
    InterCode * code = newCode(C_LABEL);
    code->x = label;
    addCode(code);
}

void addGoto(Operand * label) {
    InterCode * code = newCode(C_GOTO);
    code->x = label;
    addCode(code);
}

InterCode * rmCode(InterCode * code) {
    if (!code) return NULL;
    if (code == code_last) {
        code_last = code->prev;
        code_last->next = NULL;
        free(code);
        return code_last;
    }
    InterCode * tmp = code->prev;
    tmp->next = code->next;
    tmp->next->prev = tmp;
    free(code);
    return tmp;
}

void translate(pnode  root) {
    if (!root) return;
    if (root->rule == 5) {
        translateFunDec(root->l->r);
        translateCompSt(root->l->r->r);
        return;
    }
    pnode node = root->l;
    for (; node; node = node->r)
        translate(node);
}

Operand * translateVarDec(pnode root) {
    if (!root) return NULL;
    Operand * op = NULL;
    InterCode * code = NULL;
    if (root->rule == 15) {
        op = newOp(O_VARIABLE);
        op->str = root->l->type_id;
    } else if (root->rule == 16) {
        code = newCode(C_DEC);
        code->x = translateVarDec(root->l);
        op = newOp(O_SIZE);
        op->value = root->l->r->r->type_int;
        code->y = op;
        addCode(code);
    }
    return op;
}

void translateFunDec(pnode  root) {
    if (!root) return;
    InterCode * code = newCode(C_FUNC);
    code->x = newOp(O_FUNC);
    code->x->str = root->l->type_id;
    addCode(code);
    if (root->rule == 17)   // FunDec -> ID LP VarList RP
        translateVarList(root->l->r->r);
}

void translateVarList(pnode  root) {
    if (!root) return;
    InterCode * code = newCode(C_PARAM);
    code->x = newOp(O_VARIABLE);
    code->x->str = root->l->l->r->l->type_id;
    addCode(code);
    if (root->rule == 19)
        translateVarList(root->l->r->r);
}

void translateCompSt(pnode  root) {
    if (!root) return;
    translateDefList(root->l->r);
    translateStmtList(root->l->r->r);
}

void translateDefList(pnode root) {
    if (!root || root->rule == 32) return;
    translateDecList(root->l->l->r);
    translateDefList(root->l->r);
}

void translateDecList(pnode root) {
    if (!root) return;
    translateDec(root->l);
    if (root->rule == 35) translateDecList(root->l->r->r);
}

void translateDec(pnode root) {
    if (!root) return;
    InterCode *code;
    if (root->rule == 36) {
        translateVarDec(root->l);
    } else if (root->rule == 37) {
        code = newCode(C_ASSIGN);
        code->x = translateVarDec(root->l);
        code->y = translateExp(root->l->r->r);
        addCode(code);
    }
}

void translateStmtList(pnode  root) {
    if (!root || root->rule == 24) return;
    translateStmt(root->l);
    translateStmtList(root->l->r);
}

Operand * translateExp(pnode  root) {
    if (!root) return NULL;
    Operand * op1, *op2;
    Operand * cur_op = NULL;
    InterCode * cur_code = NULL;
    switch (root->rule) {
        case 38:    // Exp -> Exp ASSIGNOP Exp
            cur_code = newCode(C_ASSIGN);
            cur_code->x = translateExp(root->l);
            cur_code->y = translateExp(root->l->r->r);
            break;
        // case 39:
        //     break;
        // case 40:
        //     break;
        // case 41:
        //     break;
        case 42:    // Exp -> Exp PLUS Exp
            op1 = translateExp(root->l);
            op2 = translateExp(root->l->r->r);
            if (op1->kind == O_CONSTANT && op2->kind == O_CONSTANT) {
                cur_op = newOp(O_CONSTANT);
                cur_op->value = op1->value + op2->value;
                free(op1);
                free(op2);
                return cur_op;
            }
            cur_code = newCode(C_ADD);
            cur_code->y = op1;
            cur_code->z = op2;
            cur_code->x = newTemp();
            break;
        case 43:    // Exp -> Exp MINUS Exp
            op1 = translateExp(root->l);
            op2 = translateExp(root->l->r->r);
            if (op1->kind == O_CONSTANT && op2->kind == O_CONSTANT) {
                cur_op = newOp(O_CONSTANT);
                cur_op->value = op1->value - op2->value;
                free(op1);
                free(op2);
                return cur_op;
            }
            cur_code = newCode(C_SUB);
            cur_code->y = op1;
            cur_code->z = op2;
            cur_code->x = newTemp();
            break;
        case 44:    // Exp -> Exp STAR Exp
            op1 = translateExp(root->l);
            op2 = translateExp(root->l->r->r);
            if (op1->kind == O_CONSTANT && op2->kind == O_CONSTANT) {
                cur_op = newOp(O_CONSTANT);
                cur_op->value = op1->value * op2->value;
                free(op1);
                free(op2);
                return cur_op;
            }
            cur_code = newCode(C_MUL);
            cur_code->y = op1;
            cur_code->z = op2;
            cur_code->x = newTemp();
            break;
        case 45:    // Exp -> Exp DIV Exp
            op1 = translateExp(root->l);
            op2 = translateExp(root->l->r->r);
            if (op1->kind == O_CONSTANT && op2->kind == O_CONSTANT) {
                cur_op = newOp(O_CONSTANT);
                cur_op->value = op1->value / op2->value;
                free(op1);
                free(op2);
                return cur_op;
            }
            cur_code = newCode(C_DIV);
            cur_code->y = op1;
            cur_code->z = op2;
            cur_code->x = newTemp();
            break;
        case 46:    // Exp -> LP Exp RP
            return translateExp(root->l->r);
            break;
        case 47:    // Exp -> MINUS Exp
            cur_code = newCode(C_SUB);
            cur_code->y = newOp(O_CONSTANT);
            cur_code->y->value = 0;
            cur_code->z = translateExp(root->l->r);
            cur_code->x = newTemp();
            break;
        // case 48:
        //     break;
        case 49:    // Exp -> ID LP Args RP
            cur_code = newCode(C_CALL);
            translateArgs(root->l->r->r);
            cur_code->y = newOp(O_FUNC);
            cur_code->y->str = root->l->type_id;
            cur_code->x = newTemp();
            break;
        case 50:    // Exp -> ID LP RP
            cur_code = newCode(C_CALL);
            cur_code->y = newOp(O_FUNC);
            cur_code->y->str = root->l->type_id;
            cur_code->x = newTemp();
            break;
        case 51:    // Exp -> Exp LB Exp RB
            op1 = translateExp(root->l->r->r);
            if (op1->kind == O_CONSTANT) {
                op1->value *= 4;
                cur_code = newCode(C_ASSIGN);
                cur_code->y = op1;
            } else {
                cur_code = newCode(C_MUL);
                cur_code->y = op1;
                cur_code->z = newOp(O_CONSTANT);
                cur_code->z->value = 4;
            }
            cur_op = newTemp();
            cur_code->x = cur_op;
            addCode(cur_code);
            
            cur_code = newCode(C_ADD);
            cur_code->z = cur_op;
            cur_op = newOp(O_ADDR);
            cur_op->str = translateExp(root->l)->str;
            cur_code->y = cur_op;
            cur_code->x = newTemp();
            addCode(cur_code);
            cur_op = newOp(O_STAR);
            cur_op->str = cur_code->x->str;
            return cur_op;
            break;
        // case 52:
        //     break;
        case 53:    // Exp -> ID
            cur_op = newOp(O_VARIABLE);
            cur_op->str = root->l->type_id;
            return cur_op;
            break;
        case 54:    // Exp -> INT
            cur_op = newOp(O_CONSTANT);
            cur_op->value = root->l->type_int;
            return cur_op;
            break;
        // case 55:
        //     break;
        default:
            break;
    }
    addCode(cur_code);
    return cur_code->x;
}

void translateStmt(pnode  root) {
    if (!root) return;
    InterCode *code;
    Operand *label1, *label2, *label3;
    switch (root->rule) {
        case 25:    // Stmt -> Exp SEMI
            translateExp(root->l);
            break;
        case 26:    // Stmt -> CompSt
            translateCompSt(root->l);
            break;
        case 27:    // Stmt -> RETURN Exp SEMI
            code = newCode(C_RETURN);
            code->x = translateExp(root->l->r);
            addCode(code);
            break;
        case 28:    // Stmt -> IF LP Exp RP Stmt
            label1 = newLabel();
            label2 = newLabel();
            translateCond(root->l->r->r, label1, label2);
            addLabel(label1);
            translateStmt(root->l->r->r->r->r);
            addLabel(label2);
            break;
        case 29:    // Stmt -> IF LP Exp RP Stmt ELSE Stmt
            label1 = newLabel();
            label2 = newLabel();
            label3 = newLabel();
            translateCond(root->l->r->r, label1, label2);
            addLabel(label1);
            translateStmt(root->l->r->r->r->r);
            addGoto(label3);
            addLabel(label2);
            translateStmt(root->l->r->r->r->r->r->r);
            addLabel(label3);
            break;
        case 30:    // Stmt -> WHILE LP Exp RP Stmt
            label1 = newLabel();
            label2 = newLabel();
            label3 = newLabel();
            addLabel(label1);
            translateCond(root->l->r->r, label2, label3);
            addLabel(label2);
            translateStmt(root->l->r->r->r->r);
            addGoto(label1);
            addLabel(label3);
            break;
    }
}

void translateCond(pnode root, Operand *label_true, Operand *label_false) {
    if (!root) return;
    InterCode * code;
    Operand *label1;
    Operand *t1, *t2;
    switch (root->rule) {
        case 39:    // Exp -> Exp AND Exp
            label1 = newLabel();
            translateCond(root->l, label1, label_false);
            addLabel(label1);
            translateCond(root->l->r->r, label_true, label_false);
            break;
        case 40:    // Exp -> Exp OR Exp
            label1 = newLabel();
            translateCond(root->l, label_true, label1);
            addLabel(label1);
            translateCond(root->l->r->r, label_true, label_false);
            break;
        case 41:    // Exp -> Exp RELOP Exp
            t1 = translateExp(root->l);
            t2 = translateExp(root->l->r->r);
            code = newCode(C_IFGT);
            code->x = t1;
            code->y = t2;
            code->z = label_true;
            code->relop = newOp(O_RELOP);
            code->relop->str = root->l->r->type_id;
            addCode(code);
            addGoto(label_false);
            break;
        case 48:    // Exp -> NOT Exp
            translateCond(root->l->r, label_false, label_true);
            break;
        default:
            t1 = translateExp(root);
            code = newCode(C_IFGT);
            code->x = t1;
            code->y = newOp(O_CONSTANT);
            code->y->value = 0;
            code->z = label_true;
            code->relop = newOp(O_RELOP);
            code->relop->str = "!=";
            addCode(code);
            addGoto(label_false);
            break;
    }
}

void translateArgs(pnode  root) {
    if (!root) return;
    InterCode * cur_code = newCode(C_ARG);
    cur_code->x = translateExp(root->l);
    if (root->rule == 56) translateArgs(root->l->r->r);
    addCode(cur_code);
}

void optimize() {
    InterCode *code1, *code2;
    int ret = 0;
    int *label_list = (int *)malloc(label_no * sizeof(int));
    memset(label_list, 0, label_no * sizeof(int));

    // 删除return后面无法到达的语句
    code1 = code_head;
    while ((code1 = code1->next)) {
        if (ret && (code1->kind != C_FUNC) && (code1->kind != C_LABEL)) {
            code1 = rmCode(code1);
            continue;
        }
        ret = 0;
        switch (code1->kind) {
            case C_RETURN:
                ret = 1;
                break;
            case C_GOTO:
                label_list[code1->x->var_no] = 1;
                break;
            case C_IFGT:
                label_list[code1->z->var_no] = 1;
                break;
            default:
                break;
        }
    }

    code1 = code_head;
    while ((code1 = code1->next)) {
        if (code1->kind == C_ASSIGN) {
            code2 = code1->prev;
            if (code1->y->kind == O_TEMP && code1->y == code2->x) {
                code2->x = code1->x;
                code1 = rmCode(code1);
                continue;
            } 
            code2 = code1->next;
            if (code2 && code1->x->kind == O_TEMP) {
                if (code2->y == code1->x) {
                    code2->y = code1->y;
                    code1 = rmCode(code1);
                } else if (code2->z == code1->x) {
                    code2->z = code1->y;
                    code1 = rmCode(code1);
                }
            }
        } else if (code1->kind == C_LABEL) {    // 删除用不到的标号
            if (label_list[code1->x->value] == 0)
                code1 = rmCode(code1);
        }
    }

    // 修改write和read函数
    code1 = code_head;
    while ((code1 = code1->next)) {
        if (code1->kind == C_CALL) {
            if (!strcmp(code1->y->str, "write")) {
                code2 = code1->prev;
                code2->kind = C_WRITE;
                code1 = rmCode(code1);
            } else if (!strcmp(code1->y->str, "read")) {
                code1->kind = C_READ;
                code1->y = NULL;
            }
        }
    }
}

void displayOperand(Operand * op) {
    if (op == NULL) return;
    switch (op->kind) {
        case O_VARIABLE:
        case O_TEMP:
        case O_FUNC:
        case O_RELOP:
            printf("%s", op->str);
            break;
        case O_CONSTANT:
            printf("#%d", op->value);
            break;
        case O_LABEL:
            printf("label%d", op->var_no);
            break;
        case O_SIZE:
            printf("%d", op->value * 4);
            break;
        case O_ADDR:
            printf("&%s", op->str);
            break;
        case O_STAR:
            printf("*%s", op->str);
        default:
            break;
    }
}

void displayInterCode() {
    InterCode *cur = code_head;
    while ((cur = cur->next)) {
        switch (cur->kind) {
            case C_LABEL:
                printf("LABEL ");
                displayOperand(cur->x);
                printf(" :");
                break;
            case C_FUNC:
                printf("FUNCTION ");
                displayOperand(cur->x);
                printf(" :");
                break;
            case C_ASSIGN:
                displayOperand(cur->x);
                printf(" := ");
                displayOperand(cur->y);
                break;
            case C_ADD:
                displayOperand(cur->x);
                printf(" := ");
                displayOperand(cur->y);
                printf(" + ");
                displayOperand(cur->z);
                break;
            case C_SUB:
                displayOperand(cur->x);
                printf(" := ");
                displayOperand(cur->y);
                printf(" - ");
                displayOperand(cur->z);
                break;
            case C_MUL:
                displayOperand(cur->x);
                printf(" := ");
                displayOperand(cur->y);
                printf(" * ");
                displayOperand(cur->z);
                break;
            case C_DIV:
                displayOperand(cur->x);
                printf(" := ");
                displayOperand(cur->y);
                printf(" / ");
                displayOperand(cur->z);
                break;
            case C_GOTO:
                printf("GOTO ");
                displayOperand(cur->x);
                break;
            case C_IFGT:
                printf("IF ");
                displayOperand(cur->x);
                putchar(' ');
                displayOperand(cur->relop);
                putchar(' ');
                displayOperand(cur->y);
                printf(" GOTO ");
                displayOperand(cur->z);
                break;
            case C_RETURN:
                printf("RETURN ");
                displayOperand(cur->x);
                break;
            case C_DEC:
                printf("DEC ");
                displayOperand(cur->x);
                putchar(' ');
                displayOperand(cur->y);
                break;
            case C_ARG:
                printf("ARG ");
                displayOperand(cur->x);
                break;
            case C_CALL:
                displayOperand(cur->x);
                printf(" := CALL ");
                displayOperand(cur->y);
                break;
            case C_PARAM:
                printf("PARAM ");
                displayOperand(cur->x);
                break;
            case C_READ:
                printf("READ ");
                displayOperand(cur->x);
                break;
            case C_WRITE:
                printf("WRITE ");
                displayOperand(cur->x);
                break;
            default:
                break;
        }
        putchar('\n');
    }
}

void generateInterCode(pnode  root) {
    code_head = code_last = newCode(0);
    translate(root);
    optimize();
    displayInterCode();
}

