#ifndef TARGET_H_
#define TARGET_H_
#include "inter.h"


void createVar(InterCode *);
void addParam(Operand *);
void addArr(InterCode *);
void addVar(Operand *);
int findPos(char *);
void printPos();

void to_reg(Operand *, int);
void to_mem(Operand *, int);

void buildFun(InterCode *);
void transCode(InterCode *);
void generateTarget(InterCode *);

#endif
