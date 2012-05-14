#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct symtab_var;
struct symtab_class;
struct symtab_method;

typedef enum {
	VAR_UNKNOWN = 0, // default should flip out
	VAR_VOID,
	VAR_STRING_ARRAY,
	VAR_BOOL,
	VAR_INT,
	VAR_INT_ARRAY,
	VAR_CLASS
} ast_var_type;

typedef struct {
	ast_var_type type;
	const char *classname; // Used by VAR_CLASS
	struct symtab_class *class; // Used by VAR_CLASS during typecheck
} ast_type;

typedef enum {
	INT_CONST,
	BOOL_CONST,
	VARNAME,
	THIS_PTR,
	NOT_EXPR,
	NEW_CLASS,
	NEW_INT_ARRAY,
	ARRAY_LENGTH,
	ARRAY_INDEX,
	METHOD_CALL,
	BINOP,
} ast_expr_type;

typedef struct ast_expr {
	int lineno;
	ast_expr_type type;
	ast_type expr_type; // Used during typecheck
	union {
		int32_t int_const; // INT_CONST
		bool bool_const; // BOOL_CONST
		struct {
			const char *id; // VARNAME, NEW_CLASS
			struct symtab_var *bind_var;
			struct symtab_class *bind_class;
		};
		struct ast_expr *expr; // NOT_EXPR / NEW_INT_ARRAY / ARRAY_LENGTH
		struct { // METHOD_CALL
			struct ast_expr *object;
			const char *method;
			struct ast_expr *exp_list;
			struct symtab_method *bind_method;
		};
		struct { // BINOP
			struct ast_expr *lhs, *rhs;
			int oper;
		};
		struct { // ARRAY_INDEX
			struct ast_expr *array, *array_index;
		};
	};
	struct ast_expr *next; // Non-NULL => ExpList
} ast_expr;

typedef enum {
	BLOCK,
	IF_ELSE,
	WHILE_STMT,
	SYS_OUT,
	VAR_ASSIGN,
	ARRAY_ASSIGN,
} ast_stmt_type;

typedef struct ast_stmt {
	int lineno;
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
		struct { // VAR_ASSIGN / ARRAY_ASSIGN
			const char *id;
			struct symtab_var *bind;
			ast_expr *array_index; // ARRAY_ASSIGN
			ast_expr *assign_expr; // VAR_ASSIGN / ARRAY_ASSIGN
		};
		struct ast_expr *expr; // SYSO
	};
	struct ast_stmt *next; // Non-NULL => StmtList
} ast_stmt;


typedef struct ast_vardecl {
	int lineno;
	ast_type type;

	const char *id;

	struct symtab_var *bind;

	struct ast_vardecl *next; // Non-NULL => VarList
} ast_vardecl;

typedef struct ast_methoddecl {
	int lineno;
	ast_type type;
	const char *id;
	ast_vardecl *params;
	ast_vardecl *var_decl;
	ast_stmt *body;
	ast_expr *return_expr;

	struct symtab_method *bind;

	struct ast_methoddecl *next; // MethodList
} ast_methoddecl;

typedef struct ast_classdecl {
	int lineno;
	const char *id;

	ast_vardecl *fields;
	ast_methoddecl *methods;

	struct symtab_class *bind;

	struct ast_classdecl *next; // ClassList
} ast_classdecl;

typedef struct
{
	int lineno;
	const char *id;
	ast_methoddecl *method;
} ast_mainclass;

typedef struct {
	ast_mainclass main_class;
	ast_classdecl *class_list;
} ast_program;

#define AST_EXPR_EMPTY(name, expr_type) ast_expr *name = malloc(sizeof(ast_expr)); name->type = expr_type; name->lineno = yylineno; name->next = NULL;
#define AST_EXPR(name, type, assign) AST_EXPR_EMPTY(name, type) name->assign;

#define AST_STMT_EMPTY(name, stmt_type) ast_stmt *name = malloc(sizeof(ast_stmt)); name->type = stmt_type; name->lineno = yylineno; name->next = NULL;
#define AST_STMT(name, type, assign) AST_STMT_EMPTY(name, type) name->assign;

#define AST_VARDECL(name, var_type, var_id) ast_vardecl *name = malloc(sizeof(ast_vardecl)); name->type = var_type; name->id = var_id; name->lineno = yylineno; name->next = NULL;

#define AST_TYPE(name, var_type) ast_type name; name.type = var_type;
#define AST_CLASS(name, class_id) AST_TYPE(name, VAR_CLASS); name.classname = class_id;

#define AST_METHODDECL(name, method_type, method_id, method_params, method_vars, method_body, method_return) \
	ast_methoddecl *name = malloc(sizeof(ast_methoddecl)); name->next = NULL; name->type = method_type; \
	name->id = method_id; name->params = method_params; name->var_decl = method_vars; \
	name->body = method_body; name->lineno = yylineno; name->return_expr = method_return;

#define AST_CLASSDECL(name, class_id, class_fields, class_methods) ast_classdecl *name = malloc(sizeof(ast_classdecl)); \
	name->lineno = yylineno; name->next = NULL; name->id = class_id; name->fields = class_fields; name->methods = class_methods;

#define AST_PROGRAM(name, main, classlist) \
	ast_program name; name.main_class = main; name.class_list = classlist;

void ast_expr_print(FILE *file, int indent_level, ast_expr *expr);
void ast_stmt_print(FILE *file, int indent_level, ast_stmt *stmt);
void ast_vardecl_print(FILE *file, int indent_level, ast_vardecl *decl);
void ast_method_print(FILE *file, int indent_level, ast_methoddecl *method);
void ast_class_print(FILE *file, int indent_level, ast_classdecl *class);
void ast_main_print(FILE *file, int indent_level, ast_mainclass *main_class);
void ast_program_print(FILE *file, int indent_level, ast_program *program);
const char *ast_type_str(ast_type type);


#endif

