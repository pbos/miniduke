#include "symtab.h"

#include "miniduke.h"
#include <stdlib.h>

extern void print_indent(FILE *file, int indent_level);

symtab_var *symtab_init_vars(ast_vardecl *decl)
{
	if(decl == NULL)
		return NULL;

	// CHECK FOR DUPLICATES

	symtab_var *var = calloc(1, sizeof(symtab_var));

	var->next = symtab_init_vars(decl->next);
	var->type = decl->type;
	var->id = decl->id;

	return var;
}

symtab_method *symtab_init_methods(ast_methoddecl *methods)
{
	if(methods == NULL)
		return NULL;

	// CHECK FOR DUPLICATES

	symtab_method *method = calloc(1, sizeof(symtab_method));

	method->next = symtab_init_methods(methods->next);

	method->params = symtab_init_vars(methods->params);
	method->locals = symtab_init_vars(methods->var_decl);

	method->id = methods->id;
	method->type = methods->type;

	return method;
}

symtab_class *symtab_init_classes(ast_classdecl *class)
{
	if(class == NULL)
		return NULL;

	symtab_class *symclass = calloc(1, sizeof(symtab_class));

	symclass->id = class->id;
	symclass->fields = symtab_init_vars(class->fields);
	symclass->methods = symtab_init_methods(class->methods);

	symclass->next = symtab_init_classes(class->next);

	return symclass;
}

void symtab_init()
{
	// Read main class
	md_symtab.main_class = md_ast.main_class.id;

	// Read main method

	// Parse other classes
	md_symtab.classes = symtab_init_classes(md_ast.class_list);
}

extern const char *ast_type_str(ast_type type);

void symtab_print_vars(FILE *file, int indent, symtab_var *vars)
{
	if(vars == NULL)
		return;

	print_indent(file, indent);
	fprintf(file, "%s : %s\n", vars->id, ast_type_str(vars->type));

	symtab_print_vars(file, indent, vars->next);
}

void symtab_print_methods(FILE *file, int indent, symtab_method *methods)
{
	if(methods == NULL)
		return;

	print_indent(file, indent);
	fprintf(file, "%s : %s\n", methods->id, ast_type_str(methods->type));

	print_indent(file, indent+1);
	fputs("params:\n", file);
	symtab_print_vars(file, indent+2, methods->params);
	fputs("\n", file);

	print_indent(file, indent+1);
	fputs("local:\n", file);
	symtab_print_vars(file, indent+2, methods->locals);
	fputs("\n", file);

	symtab_print_methods(file, indent, methods->next);
}

void symtab_print_classes(FILE *file, symtab_class *class)
{
	if(class == NULL)
		return;

	print_indent(file, 1);
	fprintf(file, "%s:\n", class->id);

	print_indent(file, 2);
	fputs("fields:\n", file);
	symtab_print_vars(file, 3, class->fields);
	fputs("\n", file);

	print_indent(file, 2);
	fputs("methods:\n", file);
	symtab_print_methods(file, 3, class->methods);
	fputs("\n", file);

	symtab_print_classes(file, class->next);
}

void symtab_print(FILE *file)
{
	fprintf(file, "%s:\n", md_symtab.main_class);
	print_indent(file, 1);
	fprintf(file, "main:\n");
	symtab_print_vars(file, 1, md_symtab.main.locals);

	fputs("\nclasses:\n", file);

	symtab_print_classes(file, md_symtab.classes);
}

