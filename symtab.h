#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

struct symtab_class;
struct symtab_method;

// field => sym_class as parent
// param/vars => sym_method as parent
typedef struct
{
	enum {
	SYM_METHOD,
	SYM_CLASS,
	} type;

	union {
		struct symtab_class *class;
		struct symtab_method *method;
	};
} symtab_parent;

typedef struct symtab_var
{
	int lineno;
	ast_type type;
	const char *id;

	symtab_parent parent;

	int var_reg; // used by jasmin

	struct symtab_var *next;
} symtab_var;

typedef struct symtab_method {
	int lineno;
	ast_type type;
	const char *id;

	symtab_var *params;
	symtab_var *locals;

	struct symtab_class *class;
	struct symtab_method *next;
} symtab_method;

typedef struct symtab_class {
	ast_type type;
	int lineno;
	const char *id;
	symtab_var *fields;
	symtab_method *methods;
	
	struct symtab_class *next;
} symtab_class;

typedef struct
{
	symtab_class main_class;

	symtab_class *classes;
} symtab_program;

void symtab_init();
void symtab_print(FILE *file);

symtab_class *symtab_find_class(int lineno, const char *id);
symtab_var *symtab_find_var(int lineno, ast_methoddecl *ast_method, const char *id);

#endif

