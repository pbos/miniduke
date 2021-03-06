%{
	#include <stdio.h>
	#include <stdint.h>
	#include <inttypes.h>
	#include <string.h>
	#include <stdlib.h>
	#include "miniduke.h"

	extern int yylex();
	extern int yylineno;
	void yyerror(const char *s) { md_error(yylineno, "%s", s); }
%}

%code requires{
	#include "ast.h"
}

%union{
	char *id;
	int token;
	ast_expr *expr;
	ast_stmt *stmt;
	ast_vardecl *decl;
	ast_methoddecl *method;
	ast_classdecl *class;
	ast_mainclass main;
	ast_type type;
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

/*%type <token> Op*/
%type <expr> Exp ExpList ExpRest ExpRestList NewIntArray Multidim
%type <stmt> Stmt StmtList
%type <decl> VarDecl VarList FormalList FormalRest
%type <type> Type
%type <method> MethodDecl MethodList
%type <class> ClassDecl ClassList
%type <main> MainClass

%start Program

%error-verbose

%%

Program: MainClass ClassList {
		AST_PROGRAM(program, $1, $2)
		md_ast = program;
	}

MainClass: CLASS id LBLOCK PUBLIC STATIC VOID id LPAREN STRING LBRACK RBRACK id RPAREN LBLOCK VarList StmtList RBLOCK RBLOCK {
		if(strcmp("main", $7))
			md_error(yylineno, "MiniJava only supports the static method 'main'.");

		// main parameters
		ast_vardecl *main_params = calloc(1, sizeof(ast_vardecl));
		main_params->type.type = VAR_STRING_ARRAY;
		main_params->id = $12;
		main_params->lineno = yylineno;

		// main method
		ast_methoddecl *main_method = calloc(1, sizeof(ast_methoddecl));
		main_method->lineno = yylineno;
		main_method->params = main_params;
		main_method->type.type = VAR_VOID;
		main_method->id = $7;
		main_method->var_decl = $15;
		main_method->body = $16;

		// main class
		ast_mainclass main_class = {0};
		main_class.id = $2;
		main_class.method = main_method;
		main_class.lineno = yylineno;

		$$ = main_class;
	}

ClassList: ClassDecl ClassList { $1->next = $2; }
	| /* empty */ { $$ = NULL; }

ClassDecl: CLASS id LBLOCK VarList MethodList RBLOCK {
		AST_CLASSDECL(class, $2, $4, $5);
		$$ = class;
	}

VarList: VarList VarDecl {
		if($1 == NULL)
			$$ = $2;
		else
		{
			ast_vardecl *decl = $1;
			while(decl->next != NULL)
				decl = decl->next;
			decl->next = $2;
			$$ = $1;
		}
	}
	| /* empty */ { $$ = NULL; }

VarDecl: Type id SCOLON {
		AST_VARDECL(decl, $1, $2)
		$$ = decl;
	}

MethodList: MethodDecl MethodList { $1->next = $2; $$ = $1; }
	| /* empty */ { $$ = NULL; }

MethodDecl: PUBLIC Type id LPAREN FormalList RPAREN LBLOCK VarList StmtList RETURN Exp SCOLON RBLOCK {
	AST_METHODDECL(method, $2, $3, $5, $8, $9, $11)
	$$ = method;
}

FormalList: Type id FormalRest {
		AST_VARDECL(decl, $1, $2)
		decl->next = $3;
		$$ = decl;
	}
	| /* empty */ { $$ = NULL; }

FormalRest: COMMA Type id FormalRest{
		AST_VARDECL(decl, $2, $3)
		decl->next = $4;
		$$ = decl;
	}
	| /* empty */ { $$ = NULL; }

Type: INT LBRACK RBRACK { AST_TYPE(type, VAR_INT_ARRAY); $$ = type; }
	| BOOL { AST_TYPE(type, VAR_BOOL); $$ = type; }
	| INT { AST_TYPE(type, VAR_INT); $$ = type; }
	| id { AST_CLASS(type, $1); $$ = type; }

StmtList: Stmt StmtList { $$ = $1; $$->next = $2; }
	| /* empty */ { $$ = NULL; }

Stmt: LBLOCK StmtList RBLOCK {
		AST_STMT(stmt, BLOCK, stmt_list = $2)
		$$ = stmt;
	}
	| IF LPAREN Exp RPAREN Stmt ELSE Stmt {
		AST_STMT(stmt, IF_ELSE, cond = $3)
		stmt->true_branch = $5;
		stmt->false_branch = $7;
		$$ = stmt;
	}
	| WHILE LPAREN Exp RPAREN Stmt {
		AST_STMT(stmt, WHILE_STMT, cond = $3)
		stmt->while_branch = $5;
		$$ = stmt;
	}
	| SYSO LPAREN Exp RPAREN SCOLON {
		AST_STMT(stmt, SYS_OUT, expr = $3)
		$$ = stmt;
	}
	| id ASSIGN Exp SCOLON {
		AST_STMT(stmt, VAR_ASSIGN, id = $1)
		stmt->assign_expr = $3;
		$$ = stmt;
	}
	| id LBRACK Exp RBRACK ASSIGN Exp SCOLON {
		AST_STMT(stmt, ARRAY_ASSIGN, id = $1)
		stmt->array_index = $3;
		stmt->assign_expr = $6;
		$$ = stmt;
	}
Exp: Exp CONJ Exp {
		AST_EXPR_EMPTY(exp, BINOP)
		exp->lhs = $1;
		exp->oper = CONJ;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp LESS Exp {
		AST_EXPR_EMPTY(exp, BINOP)
		exp->lhs = $1;
		exp->oper = LESS;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp PLUS Exp {
		AST_EXPR_EMPTY(exp, BINOP)
		exp->lhs = $1;
		exp->oper = PLUS;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp MINUS Exp {
		AST_EXPR_EMPTY(exp, BINOP)
		exp->lhs = $1;
		exp->oper = MINUS;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp MULT Exp {
		AST_EXPR_EMPTY(exp, BINOP)
		exp->lhs = $1;
		exp->oper = MULT;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp LBRACK Exp RBRACK {
		AST_EXPR(exp, ARRAY_INDEX, array = $1)
		exp->array_index = $3;
		$$ = exp;
	}
	| Exp PERIOD LENGTH {
		AST_EXPR(exp, ARRAY_LENGTH, expr = $1)
		$$ = exp;
	}
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
		sprintf(buffer, "%" PRId32, exp->int_const);
		if(strcmp($1, buffer))
		{
			md_error(yylineno, "integer number too large: %s", $1);
		}
		free($1);
		$$ = exp;
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
	| NewIntArray { $$ = $1; } // Non-terminal to disallow multidim arrays
	| NEW id LPAREN RPAREN {
		AST_EXPR(exp, NEW_CLASS, id=$2)
		$$ = exp;
	}
	| NOT Exp {
		AST_EXPR(exp, NOT_EXPR, expr=$2)
		$$ = exp;
	}
	| LPAREN Exp RPAREN { $$ = $2; }

NewIntArray: NEW INT LBRACK Exp RBRACK Multidim {
		if($6 != NULL)
			md_error(yylineno, "MiniJava doesn't support multidimensional arrays.");
		AST_EXPR(exp, NEW_INT_ARRAY, expr = $4)
		$$ = exp;
	}

Multidim: LBRACK Exp RBRACK Multidim { $$ = (ast_expr *) 1; } // Non-NULL => error
	| /* empty */ { $$ = NULL; }
/*
Op: CONJ { $$ = CONJ; }
	| LESS { $$ = LESS; }
	| PLUS { $$ = PLUS;}
	| MINUS { $$ = MINUS; }
	| MULT { $$ = MULT; }
*/

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
