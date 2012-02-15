%{
#include "minijava.tab.h"
%}

%%

 /* ignore whitespace */
[ \t\n]+	;
 /* keywords */
"class" return CLASS;
"public" return PUBLIC;
"static" return STATIC;
"length" return LENGTH;
"return" return RETURN;
"if" return IF;
"else" return ELSE;
"while" return WHILE;
"new" return NEW;
"this" return THIS;
"System.out.println" return SYSO;
 /* types */
"void" return VOID;
"int" return INT;
"boolean" return BOOL;
"String" return STRING;

 /* boolean values */
"true" return TRUE;
"false" return FALSE;

 /* operators */
"||" return DISJ;
"&&" return CONJ;
"==" return EQUAL;
"!=" return NEQUAL;
"<=" return LEQ;
">=" return GEQ;
"<" return LESS;
">" return GREATER;
"+" return PLUS;
"-" return MINUS;
"*" return MULT;
"!" return NOT;

 /* symbols */
"{" return LBLOCK;
"}" return RBLOCK;
"(" return LPAREN;
")" return RPAREN;
"[" return LBRACK;
"]" return RBRACK;
";" return SCOLON;
"," return COMMA;
"." return PERIOD;

 /* identifier/numbers */
[0-9]+ yylval.id = strdup(yytext); return int_lit;
[a-zA-Z][_a-zA-Z0-9]* yylval.id = strdup(yytext); return id;

%%

int yywrap() { return -1; }

int main()
{
	while(yylex());
	return 0;
}
