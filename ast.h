#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	INT_CONST,
	BOOL_CONST,
	VARNAME,
	THIS_PTR,
	NOT_EXPR,
	NEW_CLASS,
	METHOD_CALL,
	BINOP,
} ast_expr_type;

typedef struct ast_expr {
	// TODO: int lineno;
	ast_expr_type type;
	union {
		int32_t int_const; // INT_CONST
		bool bool_const; // BOOL_CONST
		const char *id; // VARNAME, NEW_CLASS
		struct ast_expr *expr; // NOT_EXPR
		struct { // METHOD_CALL
			struct ast_expr *object;
			char *method;
			struct ast_expr *exp_list;
		};
		struct { // BINOP
			struct ast_expr *lhs, *rhs;
			int oper;
		};
	};
	struct ast_expr *next; // Non-NULL => ExpList
} ast_expr;

typedef enum {
	BLOCK,
	IF_ELSE,
	WHILE_STMT,
	SYS_OUT
} ast_stmt_type;

typedef struct ast_stmt {
	// TODO: int lineno;
	ast_stmt_type type;
	union {
		struct ast_stmt *stmt_list; // BLOCK
		struct { // IF_ELSE / WHILE
			ast_expr *cond; 
			union {
				struct ast_stmt *while_branch; // WHILE
				struct { // IF_ELSE
					struct ast_stmt *true_branch, *false_branch;
				};
			};
		};
		struct ast_expr *expr; // SYSO
	};
	struct ast_stmt *next; // Non-NULL => StmtList
} ast_stmt;

typedef enum {
	VAR_BOOL,
	VAR_INT,
	VAR_INT_VECTOR,
	VAR_CLASS
} ast_var_type;

typedef struct {
	ast_var_type type;
	char *classname; // Used by VAR_CLASS
} ast_type;

typedef struct ast_vardecl {
	ast_type type;

	char *id;

	struct ast_vardecl *next; // Non-NULL => VarList
} ast_vardecl;

#define AST_EXPR_EMPTY(name, expr_type) ast_expr *name = malloc(sizeof(ast_expr)); name->type = expr_type; name->next = NULL;
#define AST_EXPR(name, type, assign) AST_EXPR_EMPTY(name, type) name->assign;

#define AST_STMT_EMPTY(name, stmt_type) ast_stmt *name = malloc(sizeof(ast_stmt)); name->type = stmt_type; name->next = NULL;
#define AST_STMT(name, type, assign) AST_STMT_EMPTY(name, type) name->assign;

#define AST_VARDECL(name, var_type, var_id) ast_vardecl *name = malloc(sizeof(ast_vardecl)); name->type = var_type; name->id = var_id; name->next = NULL;

#define AST_TYPE(name, var_type) ast_type name; name.type = var_type;
#define AST_CLASS(name, class_id) AST_TYPE(name, VAR_CLASS); name.classname = class_id;

void ast_expr_print(FILE *file, int indent_level, ast_expr *expr);
void ast_stmt_print(FILE *file, int indent_level, ast_stmt *stmt);
void ast_vardecl_print(FILE *file, int indent_level, ast_vardecl *decl);

// TODO: void ast_type_print(FILE *file, int indent_level, ast_type *type);

#endif

