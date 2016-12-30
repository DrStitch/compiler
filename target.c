#include "target.h"
#include "inter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *var_name;
    int pos;
} Var_info;

Var_info var_list[1000];
int par_num = 0;
int tot_num = 0;
int pos = 0;

void createVar(InterCode *start) {
    par_num = 0;
    tot_num = 0;
    pos = 0;
    InterCode * code = start;

    while (code && code->kind != C_FUNC) {
        if (code->kind == C_PARAM)
            addParam(code->x);
        else if (code->kind == C_DEC)
            addArr(code);
        else {
            addVar(code->x);
            addVar(code->y);
            addVar(code->z);
        }
        code = code->next;
    }
}

void addParam(Operand *op) {
    var_list[par_num].var_name = op->str;
    var_list[par_num].pos = par_num * 4 + 8;
    par_num++;
    tot_num++;
}

void addArr(InterCode *code) {
    pos -= code->y->value * 4;
    var_list[tot_num].var_name = code->x->str;
    var_list[tot_num].pos = pos;
    tot_num++;
}

void addVar(Operand *op) {
    if (!op) return;
    if (op->kind == O_VARIABLE || op->kind == O_TEMP) {
        if (findPos(op->str) == 0) {
            pos -= 4;
            var_list[tot_num].var_name = op->str;
            var_list[tot_num].pos = pos;
            tot_num++;
        } 
    }
}

int findPos(char *s) {
    int i;
    for (i = 0; i < tot_num ; i++)
        if (!strcmp(var_list[i].var_name, s))
            return var_list[i].pos;
    return 0;
}

void printPos() {
    int i;
    for (i = 0; i < tot_num; i++)
        printf("%s: %d\n", var_list[i].var_name, var_list[i].pos);
}

void buildFun(InterCode *start) {
    createVar(start->next);
    InterCode *code;
    for (code = start; ; code = code->next) {
        transCode(code);
        if ((!code->next) || code->next->kind == C_FUNC)
            break;
    }
}

void transCode(InterCode *code) {
    switch(code->kind) {
        case C_LABEL:
            printf("  label%d:\n", code->x->var_no);
            break;
        case C_FUNC:
            printf("%s:\n", code->x->str);
            puts("    sw $fp, -4($sp)");
            puts("    addi $fp, $sp, -4");
            printf("    addi $sp, $fp, %d\n", pos);
            break;
        case C_ASSIGN:
            to_reg(code->y, 1);
            printf("    move $t0, $t1\n");
            to_mem(code->x, 0);
            break;
        case C_ADD:
            to_reg(code->y, 1);
            to_reg(code->z, 2);
            printf("    add $t0, $t1, $t2\n");
            to_mem(code->x, 0);
            break;
        case C_SUB:
            to_reg(code->y, 1);
            to_reg(code->z, 2);
            printf("    sub $t0, $t1, $t2\n");
            to_mem(code->x, 0);
            break;
        case C_MUL:
            to_reg(code->y, 1);
            to_reg(code->z, 2);
            printf("    mul $t0, $t1, $t2\n");
            to_mem(code->x, 0);
            break;
        case C_DIV:
            to_reg(code->y, 1);
            to_reg(code->z, 2);
            printf("    div $t1, $t2\n");
            printf("    mflo $t0\n");
            to_mem(code->x, 0);
            break;
        case C_GOTO:
            printf("    j label%d\n", code->x->var_no);
            break;
        case C_IFGT:
            to_reg(code->x, 0);
            to_reg(code->y, 1);
            if (!strcmp(code->relop->str, "=="))
                printf("    beq $t0, $t1, label%d\n", code->z->var_no);
            else if (!strcmp(code->relop->str, "!="))
                printf("    bne $t0, $t1, label%d\n", code->z->var_no);
            else if (!strcmp(code->relop->str, ">"))
                printf("    bgt $t0, $t1, label%d\n", code->z->var_no);
            else if (!strcmp(code->relop->str, "<"))
                printf("    blt $t0, $t1, label%d\n", code->z->var_no);
            else if (!strcmp(code->relop->str, ">="))
                printf("    bge $t0, $t1, label%d\n", code->z->var_no);
            else if (!strcmp(code->relop->str, "<="))
                printf("    ble $t0, $t1, label%d\n", code->z->var_no);
            break;
        case C_RETURN:
            to_reg(code->x, 0);
            puts("    move $v0, $t0");
            puts("    addi $sp, $fp, 4");
            puts("    lw $fp, 0($fp)");
            puts("    jr $ra");
            break;
        case C_DEC:     // 不用管
            break;
        case C_ARG:
            to_reg(code->x, 0);
            printf("    addi $sp, $sp, -4\n");
            printf("    sw $t0, 0($sp)\n");
            break;
        case C_CALL:
            printf("    addi $sp, $sp, -4\n");
            printf("    sw $ra, 0($sp)\n");
            printf("    jal %s\n", code->y->str);
            printf("    lw $ra, 0($sp)\n");
            printf("    addi $sp, $fp, %d\n", pos);
            printf("    sw $v0, %d($fp)\n", findPos(code->x->str));
            break;
        case C_PARAM:   // 不用管
            break;
        case C_READ:
            puts("    addi $sp, $sp, -4");
            puts("    sw $ra, 0($sp)");
            puts("    jal read");
            puts("    lw $ra, 0($sp)");
            puts("    addi $sp, $sp, 4");
            puts("    move $t0, $v0");
            to_mem(code->x, 0);
            break;
        case C_WRITE:
            to_reg(code->x, 0);
            puts("    move $a0, $t0");
            puts("    addi $sp, $sp, -4");
            puts("    sw $ra, 0($sp)");
            puts("    jal write");
            puts("    lw $ra, 0($sp)");
            puts("    addi $sp, $sp, 4");
            break;
    }
}

void to_reg(Operand *op, int reg) {
    switch (op->kind) {
        case O_CONSTANT:
            printf("    li $t%d, %d\n", reg, op->value);
            break;
        case O_VARIABLE:
        case O_TEMP:
            printf("    lw $t%d, %d($fp)\n", reg, findPos(op->str));
            break;
        case O_ADDR:
            printf("    add $t%d, $fp, %d\n", reg, findPos(op->str));
            break;
        case O_STAR:
            printf("    lw $t%d, %d($fp)\n", reg + 4, findPos(op->str));
            printf("    lw $t%d, 0($t%d)\n", reg, reg + 4);
            break;
        default:
            break;
    }
}

void to_mem(Operand *op, int reg) {
    switch (op->kind) {
        case O_VARIABLE:
        case O_TEMP:
            printf("    sw $t%d, %d($fp)\n", reg, findPos(op->str));
            break;
        case O_STAR:
            printf("    lw $t%d, %d($fp)\n", reg + 4, findPos(op->str));
            printf("    sw $t%d, 0($t%d)\n", reg, reg + 4);
        default:
            break;
    }
}

void generateTarget(InterCode * start) {
    InterCode * code;
    puts(".data");
    puts("_prompt: .asciiz \"Enter an integer:\"");
    puts("_ret: .asciiz \"\\n\"");
    puts(".globl main");
    puts(".text");
    puts("read:");
    puts("    li $v0, 4");
    puts("    la $a0, _prompt");
    puts("    syscall");
    puts("    li $v0, 5");
    puts("    syscall");
    puts("    jr $ra");
    puts("write:");
    puts("    li $v0, 1");
    puts("    syscall");
    puts("    li $v0, 4");
    puts("    la $a0, _ret");
    puts("    syscall");
    puts("    move $v0, $s0");
    puts("    jr $ra");
    for (code = start; code; code = code->next) {
        if (code->kind == C_FUNC) 
            buildFun(code);
    }
}
