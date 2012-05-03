#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

struct symtab_class;
struct symtab_method;

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
	int lineno;
	const char *id;
	symtab_var *fields;
	symtab_method *methods;
	
	struct symtab_class *next;
} symtab_class;

typedef struct
{
	int main_lineno;
	const char *main_class;
	symtab_method *main_method;

	symtab_class *classes;
} symtab_program;

void symtab_init();
void symtab_print(FILE *file);

#endif

