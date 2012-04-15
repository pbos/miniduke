#include "symtab.h"

#include "miniduke.h"

extern void print_indent(FILE *file, int indent_level);

void symtab_init()
{
}

void symtab_print(FILE *file)
{
	fprintf(file, "%s:\n", md_symtab.main_class);
	print_indent(file, 1);
	fprintf(file, "main:\n");
	symtab_var *var = md_symtab.main.locals;
	while(var != NULL)
	{
		print_indent(file, 2);
		fprintf(file, "%s : %s\n", var->id, ast_type_str(var->type));

		var = var->next;
	}
}

