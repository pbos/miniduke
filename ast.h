#ifndef AST_H
#define AST_H

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
	EXP_OP_EXP,
} ast_expr_type;

typedef struct ast_expr {
	ast_expr_type type;
	union {
		int32_t int_const;
		bool bool_const;
		const char *id;
		struct ast_expr *expr;
		struct {
			struct ast_expr *object;
			char *method;
			struct ast_expr *exp_list;
		};
		struct {
			struct ast_expr *lhs, *rhs;
			int oper;
		};
	};
	struct ast_expr *next; // Non-NULL => ExpList
} ast_expr;

typedef enum {
	BLOCK,
	IF_ELSE,
} ast_stmt_type;

typedef struct ast_stmt {
	ast_stmt_type type;
	union {
		struct ast_stmt *stmt_list;
		struct {
			ast_expr *if_cond;
			struct ast_stmt *true_branch, *false_branch;
		};
	};
	struct ast_stmt *next; // Non-NULL => StmtList
} ast_stmt;

#define AST_EXPR_EMPTY(name, type) ast_expr *name = malloc(sizeof(ast_expr)); name->next = NULL;
#define AST_EXPR(name, type, assign) AST_EXPR_EMPTY(name, type) name->assign;

#define AST_STMT_EMPTY(name, type) ast_stmt *name = malloc(sizeof(ast_stmt)); name->next = NULL;
#define AST_STMT(name, type, assign) AST_STMT_EMPTY(name, type) name->assign;

void ast_expr_print(FILE *file, int indent_level, ast_expr *expr);

#endif

