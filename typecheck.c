#include "typecheck.h"

#include "miniduke.h"

#include <string.h>

void typecheck_assign_class(symtab_var *var)
{
	if(var->type.type != VAR_CLASS)
		return;

	var->type.class = symtab_find_class(var->lineno, var->type.classname);
}

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

	symtab_var *var;
	for(var = method->params; var != NULL; var = var->next)
	{
		typecheck_assign_class(var);
		typecheck_varlist(method->params, var);
	}

	for(var = method->locals; var != NULL; var = var->next)
	{
		typecheck_assign_class(var);

		symtab_var *param;
		for(param = method->params; param != NULL; param = param->next)
		{
			if(param != var && !strcmp(var->id, param->id))
				md_error(var->lineno, "'%s' is already defined on line %d.", var->id, param->lineno);
		}
		typecheck_varlist(method->locals, var);
	}
}

void typecheck_classes(symtab_class *class)
{
	if(!strcmp(class->id, md_symtab.main_class.id))
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

	symtab_var *var;
	for(var = class->fields; var != NULL; var = var->next)
	{
		typecheck_assign_class(var);
		typecheck_varlist(class->fields, var);
	}

}

void typecheck_symtab()
{
	symtab_var *var;

	for(var = md_symtab.main_class.methods->locals; var != NULL; var = var->next)
	{
		if(!strcmp(md_symtab.main_class.methods->params->id, var->id))
			md_error(md_symtab.main_class.methods->params->lineno, "'%s' is already defined on line %d", var->id, var->lineno);
		typecheck_assign_class(var);
		typecheck_varlist(md_symtab.main_class.methods->locals, var);
	}

	symtab_class *class;
	for(class = md_symtab.classes; class != NULL; class = class->next)
		typecheck_classes(class);
}

