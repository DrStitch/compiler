%error-verbose
%locations
%{
#include "node.h"
#include "inter.h"
#include "target.h"

extern char *yytext;
extern FILE *yyin;
extern int yylineno;
extern int yycolumn;
extern InterCode * code_head;

pnode const_C(char *name, int line, char *con, int base);

pnode const_B(char *name, int line);
pnode root;

%}

/* declared tokens */
%token INT
%token FLOAT
%token ID

%token SEMI COMMA
%token ASSIGNOP
%token RELOP
%token PLUS MINUS STAR DIV
%token AND OR
%token DOT
%token NOT
%token LP RP LB RB LC RC
%token TYPE
%token STRUCT
%token RETURN
%token IF ELSE WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT
%left LP RP LB RB DOT

%%
/* High-level Definitions */
Program: ExtDefList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 0;
        $$->line = @1.first_line;
        strcpy($$->name, "Program");
        $$->l = $1;
        $1->r = NULL;
        root = $$;}
    ;
ExtDefList: ExtDef ExtDefList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 1;
        $$->line = @1.first_line;
        strcpy($$->name, "ExtDefList");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 2;
        $$->line = 0;
        strcpy($$->name, "ExtDefList");
        $$->l = NULL;}
    ;
ExtDef: Specifier ExtDecList SEMI {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 3;
        $$->line = @1.first_line;
        strcpy($$->name, "ExtDef");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Specifier SEMI {
        $$=(pnode)malloc(sizeof(node));
        $$->rule = 4;
        $$->line = @1.first_line;
        strcpy($$->name, "ExtDef");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | Specifier FunDec CompSt {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 5;
        $$->line = @1.first_line;
        strcpy($$->name, "ExtDef");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    ;
ExtDecList: VarDec {
        $$=(pnode)malloc(sizeof(node));
        $$->rule = 6;
        $$->line = @1.first_line;
        strcpy($$->name, "ExtDecList");
        $$->l = $1;
        $1->r = NULL;}
    | VarDec COMMA ExtDecList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 7;
        $$->line = @1.first_line;
        strcpy($$->name, "ExtDecList");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    ;
/* Specifiers */
Specifier: TYPE {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 8;
        $$->line = @1.first_line;
        strcpy($$->name, "Specifier");
        $$->l = $1;
        $1->r = NULL;}
    | StructSpecifier {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 9;
        $$->line = @1.first_line;
        strcpy($$->name, "Specifier");
        $$->l = $1;
        $1->r = NULL;}
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 10;
        $$->line = @1.first_line;
        strcpy($$->name, "StructSpecifier");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = $5;
        $5->r = NULL;}
    | STRUCT Tag {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 11;
        $$->line = @1.first_line;
        strcpy($$->name, "StructSpecifier");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    ;
OptTag: ID {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 12;
        $$->line = @1.first_line;
        strcpy($$->name, "OptTag");
        $$->l = $1;
        $1->r = NULL;}
    | {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 13;
        $$->line = 0;
        strcpy($$->name, "OptTag");
        $$->l = NULL;}
    ;
Tag: ID {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 14;
        $$->line = @1.first_line;
        strcpy($$->name, "Tag");
        $$->l = $1;
        $1->r = NULL;}
    ;
/* Declarators */
VarDec: ID {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 15;
        $$->line = @1.first_line;
        strcpy($$->name, "VarDec");
        $$->l = $1;
        $1->r = NULL;}
    | VarDec LB INT RB {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 16;
        $$->line = @1.first_line;
        strcpy($$->name, "VarDec");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = NULL;}
    ;
FunDec: ID LP VarList RP {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 17;
        $$->line = @1.first_line;
        strcpy($$->name, "FunDec");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = NULL;}
    | ID LP RP {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 18;
        $$->line = @1.first_line;
        strcpy($$->name, "FunDec");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    ;
VarList: ParamDec COMMA VarList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 19;
        $$->line = @1.first_line;
        strcpy($$->name, "VarList");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | ParamDec {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 20;
        $$->line = @1.first_line;
        strcpy($$->name, "VarList");
        $$->l = $1;
        $1->r = NULL;}
    ;
ParamDec: Specifier VarDec {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 21;
        $$->line = @1.first_line;
        strcpy($$->name, "ParamDec");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    ;
/* Statements */
CompSt: LC DefList StmtList RC {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 22;
        $$->line = @1.first_line;
        strcpy($$->name, "CompSt");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = NULL;}
    ;
StmtList: Stmt StmtList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 23;
        $$->line = @1.first_line;
        strcpy($$->name, "StmtList");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 24;
        $$->line = 0;
        strcpy($$->name, "StmtList");
        $$->l = NULL;}
    ;
Stmt: Exp SEMI {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 25;
        $$->line = @1.first_line;
        strcpy($$->name, "Stmt");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | CompSt {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 26;
        $$->line = @1.first_line;
        strcpy($$->name, "Stmt");
        $$->l = $1;
        $1->r = NULL;}
    | RETURN Exp SEMI {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 27;
        $$->line = @1.first_line;
        strcpy($$->name, "Stmt");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | IF LP Exp RP Stmt {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 28;
        $$->line = @1.first_line;
        strcpy($$->name, "Stmt");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = $5;
        $5->r = NULL;}
    | IF LP Exp RP Stmt ELSE Stmt {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 29;
        $$->line = @1.first_line;
        strcpy($$->name, "Stmt");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = $5;
        $5->r = $6;
        $6->r = $7;
        $7->r = NULL;}
    | WHILE LP Exp RP Stmt {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 30;
        $$->line = @1.first_line;
        strcpy($$->name, "Stmt");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = $5;
        $5->r = NULL;}
    ;
/* Local Definitions */
DefList: Def DefList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 31;
        $$->line = @1.first_line;
        strcpy($$->name, "DefList");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 32;
        $$->line = 0;
        strcpy($$->name, "DefList");
        $$->l = NULL;}
    ;
Def: Specifier DecList SEMI {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 33;
        $$->line = @1.first_line;
        strcpy($$->name, "Def");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    ;
DecList: Dec {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 34;
        $$->line = @1.first_line;
        strcpy($$->name, "DecList");
        $$->l = $1;
        $1->r = NULL;}
    | Dec COMMA DecList {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 35;
        $$->line = @1.first_line;
        strcpy($$->name, "DecList");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    ;
Dec: VarDec {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 36;
        $$->line = @1.first_line;
        strcpy($$->name, "Dec");
        $$->l = $1;
        $1->r = NULL;}
    | VarDec ASSIGNOP Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 37;
        $$->line = @1.first_line;
        strcpy($$->name, "Dec");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    ;
/* Expressions */
Exp: Exp ASSIGNOP Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 38;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp AND Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 39;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp OR Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 40;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp RELOP Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 41;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp PLUS Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 42;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp MINUS Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 43;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp STAR Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 44;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp DIV Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 45;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | LP Exp RP {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 46;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | MINUS Exp %prec UMINUS {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 47;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | NOT Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 48;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = NULL;}
    | ID LP Args RP {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 49;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = NULL;}
    | ID LP RP {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 50;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp LB Exp RB {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 51;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = $4;
        $4->r = NULL;}
    | Exp LB error RB {yyerrok;}
    | Exp DOT ID {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 52;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | ID {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 53;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = NULL;}
    | INT {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 54;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = NULL;}
    | FLOAT {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 55;
        $$->line = @1.first_line;
        strcpy($$->name, "Exp");
        $$->l = $1;
        $1->r = NULL;}
    ;
Args: Exp COMMA Args {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 56;
        $$->line = @1.first_line;
        strcpy($$->name, "Args");
        $$->l = $1;
        $1->r = $2;
        $2->r = $3;
        $3->r = NULL;}
    | Exp {
        $$=(pnode)malloc(sizeof(node)); 
        $$->rule = 57;
        $$->line = @1.first_line;
        strcpy($$->name, "Args");
        $$->l = $1;
        $1->r = NULL;}
    ;
%%

int main(int argc, char* argv[]) {
    FILE *fp;
    if (argc >= 2) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yyparse();
    // display(root, 0);
    // generateInterCode(root);
    // putchar('\n');
    // if (argc >= 3) {
    //     if (!(fp = freopen(argv[2], "w", stdout))) {
    //         perror(argv[2]);
    //         return 1;
    //     }
    // }
    // generateTarget(code_head->next);
    // if (argc >= 3)
    //     fclose(fp);
    return 0;
}

yyerror(char *s) {
    fprintf(stderr, "Error type B at line %d.%d: %s at %s\n", yylineno, yycolumn, s, yytext);
}
