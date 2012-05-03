#include "typecheck.h"

#include "miniduke.h"

#include <string.h>

void typecheck_varlist(symtab_var *varlist, symtab_var *var)
{
	while(varlist != NULL)
	{
		if(!strcmp(var->id, varlist->id) && varlist != var)
			md_error(varlist->lineno, "'%s' is already defined in main:%d", var->id, var->lineno);
		varlist = varlist->next;
	}
}

void typecheck()
{
	symtab_var *var;
	for(var = md_symtab.main_method->locals; var != NULL; var = var->next)
	{
		if(!strcmp(md_symtab.main_method->params->id, var->id))
			md_error(md_symtab.main_method->params->lineno, "'%s' is already defined in main:%d", var->id, var->lineno);
		typecheck_varlist(md_symtab.main_method->locals, var);
	}
}

