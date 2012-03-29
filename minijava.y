%{
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
	#include "miniduke.h"

	extern int yylex();
	extern int yylineno;
	void yyerror(const char *s) { printf("%02d: %s\n", yylineno, s); }
%}

%code requires{
	#include "ast.h"
}

%union{
	char *id;
	int token;
	ast_expr *expr;
	ast_stmt *stmt;
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
%token ASSIGN /* = */
%token SCOLON /* ; */
%token COMMA /* , */
%token PERIOD /* . */

/* identifier/numbers */
%token <id> id
%token <id> int_lit

%type <token> Op
%type <expr> Exp ExpList ExpRest ExpRestList
%type <stmt> Stmt StmtList

%start Program

%error-verbose

%%

Program: MainClass ClassList {puts("Program");}

MainClass: CLASS id LBLOCK PUBLIC STATIC VOID id LPAREN STRING LBRACK RBRACK id RPAREN LBLOCK VarList StmtList RBLOCK RBLOCK {printf("MainClass(%s)\n", $2);}

ClassList: ClassList ClassDecl
	| /* empty */

ClassDecl: CLASS id LBLOCK VarList MethodList RBLOCK {printf("ClassDecl(%s)\n", $2);}

VarList: VarList VarDecl
	| /* empty */

VarDecl: Type id SCOLON { printf("VarDecl(%s)\n", $2); }

MethodList: MethodDecl MethodList 
	| /* empty */

MethodDecl: PUBLIC Type id LPAREN FormalList RPAREN LBLOCK VarList StmtList RETURN Exp SCOLON RBLOCK {printf("MethodDecl(%s)\n", $3);}

FormalList: Type id FormalRest {puts("FormalList");}
	| /* empty */

FormalRest: FormalRest COMMA Type id {puts("FormalRest");}
	| /* empty */

Type: INT LBRACK RBRACK {puts("Type:INT[]");}
	| BOOL {puts("Type:BOOL");}
	| INT {puts("Type:INT");}
	| id { printf("Type(%s)\n",$1); }

StmtList: Stmt StmtList { $$ = $1; $$->next = $2; }
	| /* empty */ { $$ = NULL; }

Stmt: LBLOCK StmtList RBLOCK {
		AST_STMT(stmt, BLOCK, stmt_list = $2)
		$$ = stmt;
	}
	| IF LPAREN Exp RPAREN Stmt ELSE Stmt {
		AST_STMT(stmt, IF_ELSE, if_cond = $3)
		stmt->true_branch = $5;
		stmt->false_branch = $7;
		$$ = stmt;
	}
	| WHILE LPAREN Exp RPAREN Stmt {puts("Stmt:while");}
	| SYSO LPAREN Exp RPAREN SCOLON {puts("Stmt:SYSO");}
	| id ASSIGN Exp SCOLON {printf("Stmt:Assign(%s)\n", $1);}
	| id LBRACK Exp RBRACK ASSIGN Exp SCOLON {printf("Stmt:ArrayAssign(%s)\n", $1);}

Exp: Exp Op Exp {
		AST_EXPR_EMPTY(exp, EXP_OP_EXP);
		exp->lhs = $1;
		exp->oper = $2;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp LBRACK Exp RBRACK {puts("Exp:Exp[Exp]");} // What should this do?
	| Exp PERIOD LENGTH {puts("Exp:Exp.length");}
	| Exp PERIOD id LPAREN ExpList RPAREN {
		AST_EXPR_EMPTY(exp, METHOD_CALL)
		exp->object = $1;
		exp->method = $3;
		exp->exp_list = $5;
		$$ = exp;
	}
	| int_lit {
		AST_EXPR(exp, INT_CONST, int_const = atoi($1))
		char buffer[16];
		sprintf(buffer, "%d", exp->int_const);
		if(strcmp($1, buffer))
		{
			md_error(yylineno, "integer number too large: %s", $1);
		}
		free($1);
		$$=exp;
	}
	| TRUE {
		AST_EXPR(exp, BOOL_CONST, bool_const = true)
		$$ = exp;
	}
	| FALSE {
		AST_EXPR(exp, BOOL_CONST, bool_const = false)
		$$ = exp;
	}
	| id {
		AST_EXPR(exp, VARNAME, id = $1);
		$$ = exp;
	}
	| THIS {
		AST_EXPR_EMPTY(exp, THIS_PTR)
		$$ = exp;
	}
	| NEW INT LBRACK Exp RBRACK {puts("Exp:newint[]");} // Disallow multidim arrays.
	| NEW id LPAREN RPAREN {
		AST_EXPR(exp, NEW_CLASS, id=$2)
		$$ = exp;
	}
	| NOT Exp {
		AST_EXPR(exp, NOT_EXPR, expr=$2)
		$$ = exp;
	}
	| LPAREN Exp RPAREN { $$ = $2; }

Op: CONJ { $$=CONJ; }
	| LESS { $$ = LESS; }
	| PLUS { $$ = PLUS;}
	| MINUS { $$ = MINUS; }
	| MULT { $$ = MULT; }

ExpList: Exp ExpRestList {
		$$ = $1;
		$$->next = $2;
	}
	| /* empty */ { $$ = NULL; }

ExpRestList: ExpRest ExpRestList {
		$$ = $1;
		$$->next = $2;
	}
	| /* empty */ { $$ = NULL; }

ExpRest: COMMA Exp { $$ = $2; }
