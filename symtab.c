#include "symtab.h"

#include "miniduke.h"
#include <stdlib.h>

extern void print_indent(FILE *file, int indent_level);

symtab_var *symtab_init_vars(ast_vardecl *decl, symtab_parent parent)
{
	if(decl == NULL)
		return NULL;

	symtab_var *var = calloc(1, sizeof(symtab_var));

	var->lineno = decl->lineno;
	var->next = symtab_init_vars(decl->next, parent);
	var->type = decl->type;
	var->id = decl->id;

	var->parent = parent;

	return var;
}

symtab_method *symtab_init_methods(ast_methoddecl *methods, symtab_class *parent)
{
	if(methods == NULL)
		return NULL;

	symtab_method *method = calloc(1, sizeof(symtab_method));
	method->lineno = methods->lineno;

	method->next = symtab_init_methods(methods->next, parent);
	symtab_parent sym_parent;
	sym_parent.type = SYM_METHOD;
	sym_parent.method = method;
	method->params = symtab_init_vars(methods->params, sym_parent);

	method->locals = symtab_init_vars(methods->var_decl, sym_parent);
	method->class = parent;

	method->id = methods->id;
	method->type = methods->type;

	return method;
}

symtab_class *symtab_init_classes(ast_classdecl *class)
{
	if(class == NULL)
		return NULL;

	symtab_class *symclass = calloc(1, sizeof(symtab_class));
	symclass->lineno = class->lineno;

	symclass->id = class->id;
	symtab_parent parent;
	parent.type = SYM_CLASS;
	parent.class = symclass;
	symclass->fields = symtab_init_vars(class->fields, parent);
	symclass->methods = symtab_init_methods(class->methods, symclass);

	symclass->next = symtab_init_classes(class->next);

	return symclass;
}

void symtab_init()
{
	// Read main class
	md_symtab.main_class = md_ast.main_class.id;

	// Read main method
	md_symtab.main_method = symtab_init_methods(md_ast.main_class.method, NULL);

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
	symtab_print_methods(file, 1, md_symtab.main_method);

	fputs("classes:\n", file);

	symtab_print_classes(file, md_symtab.classes);
}

