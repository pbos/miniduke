#include "symtab.h"

#include "miniduke.h"

extern void print_indent(FILE *file, int indent_level);

void symtab_init()
{
}

void symtab_print(FILE *file)
{
	fprintf(file, "%s:\n", md_symtab.main_class);
}

