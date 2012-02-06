%{
#include "minijava.tab.h"
#define TOKEN(t) printf("%s ", #t);
#define OPER(t) printf("op %s", #t);
#define ID(t) printf("ID(%s) ", t);
#define NUM(t) printf("NUM(%s) ", t);
%}

%%

 /* ignore whitespace */
[ \t\n]+	;
 /* ignore comments */
\/\/.*$	;
\/\*.*\*\/ ;
"("	TOKEN(LPAREN)
")"	TOKEN(RPAREN)
"["	TOKEN(LBRACK)
"]"	TOKEN(RBRACK)
"{"	TOKEN(LCBRACK)
"}"	TOKEN(RCBRACK)
";"	TOKEN(SEMICOLON)
"."	TOKEN(PERIOD)
","	TOKEN(COMMA)
  /* operators */
"<="	OPER(LEQ)
">="	OPER(GEQ)
"<"	OPER(LESS)
">"	OPER(GREATER)
"=="	OPER(EQUALS)
"+="	OPER(ADD_COMP)
"-="	OPER(SUB_COMP)
"*="	OPER(MUL_COMP)
"/="	OPER(DIV_COMP)
"+"	OPER(ADD)
"-"	OPER(SUB)
"*"	OPER(MUL)
"/"	OPER(DIV)
"="	OPER(ASSIGNMENT)
"!"	OPER(NOT)
 /* keywords */
"static"	TOKEN(STATIC);
"class"	TOKEN(CLASS);
"if"	TOKEN(IF);
"else"	TOKEN(ELSE);
 /* types */
"void"	TOKEN(VOID);
"int"	TOKEN(INT);
"boolean"	TOKEN(BOOL);
[a-zA-Z][_a-zA-Z0-9]*	ID(yytext)
[0-9]+	NUM(yytext)
.	printf("\n---(%s)---\n", yytext);

%%

int yywrap() { return -1; }

int main()
{
	while(yylex());
}
