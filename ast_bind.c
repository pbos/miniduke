#include "ast_bind.h"

#include "miniduke.h"

void ast_bind_typecheck(int lineno, ast_type expected, ast_type actual)
{
	if(expected.type != actual.type)
		md_error(lineno, "incompatible types, found: '%s', expected '%s'.", ast_type_str(actual), ast_type_str(expected));

	if(expected.type != VAR_CLASS)
		return;

	if(strcmp(expected.classname, actual.classname))
		md_error(lineno, "incompatible types, found: '%s', expected '%s'.", ast_type_str(actual), ast_type_str(expected));
}

void ast_bind_expressions(ast_methoddecl *method, ast_expr *expr)
{
	if(expr == NULL)
		return;
	
	ast_bind_expressions(method, expr->next);
}

void ast_bind_statements(ast_methoddecl *method, ast_stmt *stmt)
{
	if(stmt == NULL)
		return;
	
	ast_bind_statements(method, stmt->next);
}

void ast_bind_methods(ast_methoddecl *methods)
{
	if(methods == NULL)
		return;

	ast_bind_statements(methods, methods->body);

	ast_bind_expressions(methods, methods->return_expr);

	ast_bind_typecheck(methods->lineno, methods->type, methods->return_expr->expr_type);

	ast_bind_methods(methods->next);
}

void ast_bind_classes(ast_classdecl *classes)
{
	if(classes == NULL)
		return;

	ast_bind_methods(classes->methods);

	ast_bind_classes(classes->next);
}

void ast_bind()
{
	// check main class

	/*
	 * code for binding mainclass
	 */

	// check other classes
	ast_bind_classes(md_ast.class_list);
}

