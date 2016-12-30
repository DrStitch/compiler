%option yylineno
%{
#include "node.h"
#include "exp.tab.h"

extern pnode const_C(char *name, int line, char *con, int base);
extern pnode const_B(char *name, int line);
int yycolumn = 1;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
dec     [+-]?[1-9][0-9]*|0
oct     [+-]?0[0-7]+
hex     [+-]?0[xX][0-9a-fA-F]+
float   [+-]?{dec}\.[0-9]+
sci     ({dec}|{float})[eE][0-9]+
id      [_a-zA-Z][_a-zA-Z0-9]*
%%
"//" {
    while(input()!='\n');
    unput('\n');}
"/*" {
    char a, b;
    a = b = input();
    while (b = input()) {
        yycolumn++;
        if (a == '\n') yycolumn = 1;
        if (a == '*' && b == '/') break;
        a = b;
    }
    yycolumn++;}
int|float {
    yylval = const_C("TYPE", yylineno, yytext, 0);
    return TYPE;}
struct {
    yylval = const_B("STRUCT", yylineno);
    return STRUCT;}
return {
    yylval = const_B("RETURN", yylineno);
    return RETURN;}
if {
    yylval = const_B("IF", yylineno);
    return IF;}
else {
    yylval = const_B("ELSE", yylineno);
    return ELSE;}
while {
    yylval = const_B("WHILE", yylineno);
    return WHILE;}
{dec} {
    yylval = const_C("INT", yylineno, yytext, 10);
    return INT;}
{oct} {
    yylval = const_C("INT", yylineno, yytext, 8);
    return INT;}
{hex} {
    yylval = const_C("INT", yylineno, yytext, 16);
    return INT;}
{float}|{sci} {
    yylval = const_C("FLOAT", yylineno, yytext, 0);
    return FLOAT;}
{id} {
    yylval = const_C("ID", yylineno, yytext, 0);
    return ID;}
";" {
    yylval = const_B("SEMI", yylineno);
    return SEMI;}
"," {
    yylval = const_B("COMMA", yylineno);
    return COMMA;}
"=" {
    yylval = const_B("ASSIGNOP", yylineno);
    return ASSIGNOP;}
>|<|>=|<=|==|!= {
    yylval = const_C("RELOP", yylineno, yytext, 0);
    return RELOP;}
"+" {
    yylval = const_B("PLUS", yylineno);
    return PLUS;}
"-" {
    yylval = const_B("MINUS", yylineno);
    return MINUS;}
"*" {
    yylval = const_B("STAR", yylineno);
    return STAR;}
"/" {
    yylval = const_B("DIV", yylineno);
    return DIV;}
"&&" {
    yylval = const_B("AND", yylineno);
    return AND;}
"||" {
    yylval = const_B("OR", yylineno);
    return OR;}
"." {
    yylval = const_B("DOT", yylineno);
    return DOT;}
"!" {
    yylval = const_B("NOT", yylineno);
    return NOT;}
"(" {
    yylval = const_B("LP", yylineno);
    return LP;}
")" {
    yylval = const_B("RP", yylineno);
    return RP;}
"[" {
    yylval = const_B("LB", yylineno);
    return LB;}
"]" {
    yylval = const_B("RB", yylineno);
    return RB;}
"{" {
    yylval = const_B("LC", yylineno);
    return LC;}
"}" {
    yylval = const_B("RC", yylineno);
    return RC;}
\n {yycolumn = 1;}
" " {}
\t {}
. {printf("Error type A at Line %d.%d: %s\n", yylineno, yycolumn, yytext);}

%%
/*
int main(int argc, char* argv[]) {
    int token_type;
    if (argc > 1)
        yyin = fopen(argv[1], "r");
    else
        yyin = stdin;
    while (token_type = yylex());
    return 0;
}

int yywrap() {
    return 1;
}
*/
