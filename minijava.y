%union{
	char *id;
	int number;
	int token;
}

/* keywords */
%token CLASS /* class" */
%token PUBLIC /* public */
%token STATIC /* static */
%token MAIN /* main */
%token RETURN /* return */
%token IF /* if */
%token WHILE /* while */

/* types */
%token INT /* int */
%token BOOL /* boolean */

/* boolean values */
%token TRUE /* true */
%token FALSE /* false */

/* operators */
/* precedence taken from http://docs.oracle.com/javase/tutorial/java/nutsandbolts/operators.html */
%left DISJ /* || */
%left CONJ /* && */
%left EQUAL NEQUAL /* == != */
%left LESS GREATER LEQ GEQ /* < > <= >= */
%left PLUS MINUS /* + - */
%left MULT /* * */

/* symbols */
%token LBLOCK /* { */
%token RBLOCK /* } */
%token LPAREN /* ( */
%token RPAREN /* ) */
%token LBRACK /* [ */
%token RBRACK /* ] */
%token SCOLON /* ; */
%token COMMA /* , */

%%
