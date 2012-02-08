%{
#include "calc.tab.h"
#define NUM(x) {yylval.value = atoi(x); return INT;}
#define OPER(x) {yylval.oper = x; return x;}
#define TOKEN(x) OPER(x)
%}

%%

 /* ignore whitespace */
[ \t]+	;
[\n] TOKEN('\n')
"("	TOKEN('(')
")"	TOKEN(')')
"+"	OPER('+')
"-"	OPER('-')
"*"	OPER('*')
"/"	OPER('/')
[0-9]+	NUM(yytext)
<<EOF>> return FILE_EOF;
[^ \t]+ printf("unknown token: %s\n", yytext); yyterminate();

%%

int yywrap() { return -1; }

void yyerror() { }
int main()
{
	yyparse();
	return 0;
}
