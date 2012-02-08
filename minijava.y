%union{
	char *id;
}

/* keywords */
%token CLASS /* class" */
%token PUBLIC /* public */
%token STATIC /* static */
%token LENGTH /* length */
%token RETURN /* return */
%token IF /* if */
%token ELSE /* else */
%token WHILE /* while */
%token NEW /* new */
%token THIS /* this */
%token SYSO /* System.out.println */

/* types */
%token VOID /* void */
%token INT /* int */
%token BOOL /* boolean */
%token STRING /* String */

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
%left NOT /* ! */

/* symbols */
%token LBLOCK /* { */
%token RBLOCK /* } */
%token LPAREN /* ( */
%token RPAREN /* ) */
%token LBRACK /* [ */
%token RBRACK /* ] */
%token SCOLON /* ; */
%token COMMA /* , */
%token PERIOD /* . */
%token COLON /* , */

/* identifier/numbers */
%token <id> id
%token <num> int_lit

%start Program
%%

Program: MainClass ClassList

MainClass: CLASS id LBLOCK PUBLIC STATIC VOID id LPAREN STRING LBRACK RBRACK id LBLOCK VarList StmtList

ClassList: ClassList ClassDecl
	| /* empty */

ClassDecl: CLASS id LBLOCK VarList MethodList RBLOCK

VarList: VarList VarDecl
	| /* empty */

VarDecl: Type id SCOLON

MethodList: MethodList MethodDecl
	| /* empty */

MethodDecl: PUBLIC Type id LPAREN FormalList RPAREN LBLOCK VarList StmtList RETURN Exp SCOLON RBLOCK

FormalList: Type id FormalRest
	| /* empty */

FormalRest: FormalRest COMMA Type id
	| /* empty */

Type: INT LBRACK RBRACK
	| BOOL
	| INT
	| id

StmtList: StmtList Stmt
	|

Stmt: LBLOCK StmtList RBLOCK
	| IF LPAREN Exp RPAREN Stmt ELSE Stmt
	| WHILE LPAREN Exp RPAREN Stmt
	| SYSO LPAREN Exp RPAREN COLON

Exp: Exp Op Exp
	| Exp LBRACK Exp RBRACK
	| Exp PERIOD LENGTH
	| Exp PERIOD id LPAREN ExpList RPAREN
	| int_lit
	| TRUE
	| FALSE
	| id
	| THIS
	| NEW INT LBRACK Exp RBRACK
	| NEW id LPAREN RPAREN
	| NOT Exp
	| LPAREN Exp RPAREN

Op: CONJ
	| LESS
	| PLUS
	| MINUS
	| MULT

ExpList: Exp ExpRest
	| /* empty */

ExpRest: COMMA Exp

