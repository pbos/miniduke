#include "typecheck.h"

#include "miniduke.h"

#include <string.h>

void typecheck_varlist(symtab_var *varlist, symtab_var *var)
{
	while(varlist != NULL)
	{
		if(varlist != var && !strcmp(var->id, varlist->id))
			md_error(varlist->lineno, "'%s' is already defined on line %d.", var->id, var->lineno);
		varlist = varlist->next;
	}
}

void typecheck_method(symtab_method *method)
{
	symtab_method *methods;

	for(methods = method->class->methods; methods != NULL; methods = methods->next)
	{
		if(methods != method && !strcmp(method->id, methods->id))
			md_error(methods->lineno, "%s is already defined in %s.", methods->id, methods->class->id);
	}
}

void typecheck_classes(symtab_class *class)
{
	if(!strcmp(class->id, md_symtab.main_class))
		md_error(class->lineno, "duplicate class: %s defined on line %d.", class->id, class->lineno);

	symtab_class *classlist;
	
	for(classlist = md_symtab.classes; classlist != NULL; classlist = classlist->next)
	{
		if(classlist != class && !strcmp(classlist->id, class->id))
			md_error(classlist->lineno, "duplicate class: %s defined on line %d.", class->id, class->lineno);
	}

	symtab_method *method;
	for(method = class->methods; method != NULL; method = method->next)
		typecheck_method(method);
}

void typecheck()
{
	symtab_var *var;
	for(var = md_symtab.main_method->locals; var != NULL; var = var->next)
	{
		if(!strcmp(md_symtab.main_method->params->id, var->id))
			md_error(md_symtab.main_method->params->lineno, "'%s' is already defined on line %d", var->id, var->lineno);
		typecheck_varlist(md_symtab.main_method->locals, var);
	}

	symtab_class *class;
	for(class = md_symtab.classes; class != NULL; class = class->next)
		typecheck_classes(class);
}

