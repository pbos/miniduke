#include "ast_bind.h"

#include "miniduke.h"
#include "ast.h"

#include <string.h>

void ast_bind_typecheck(int lineno, ast_type expected, ast_type actual)
{
	if(expected.type != actual.type)
		md_error(lineno, "incompatible types, found: '%s', expected '%s'.", ast_type_str(actual), ast_type_str(expected));

	if(expected.type != VAR_CLASS)
		return;

	if(strcmp(expected.classname, actual.classname))
		md_error(lineno, "incompatible types, found: '%s', expected '%s'.", ast_type_str(actual), ast_type_str(expected));
}

// forward decl
void ast_bind_expressions(ast_methoddecl *method, ast_expr *expr);

void ast_bind_methodcall(ast_methoddecl *method, ast_expr *expr)
{
	ast_bind_expressions(method, expr->object);

	if(expr->object->expr_type.type != VAR_CLASS)
		md_error(expr->object->lineno, "method call, expected class, found '%s'.", ast_type_str(expr->object->expr_type));

	for(expr->bind_method = expr->object->expr_type.class->methods;
		expr->bind_method != NULL;
		expr->bind_method = expr->bind_method->next)
	{
		if(!strcmp(expr->method, expr->bind_method->id))
			break;
	}
	if(expr->bind_method == NULL)
		md_error(expr->lineno, "cannot find method '%s' in class '%s'.", expr->method, ast_type_str(expr->object->expr_type));

	ast_bind_expressions(method, expr->exp_list);

	ast_expr *param_expr;
	symtab_var *param;
	for(param_expr = expr->exp_list, param = expr->bind_method->params;
		param_expr != NULL && param != NULL;
		param_expr = param_expr->next, param = param->next)
	{
		ast_bind_typecheck(param_expr->lineno, param_expr->expr_type, param->type);
	}
	if(param_expr != NULL || param != NULL)
		md_error(expr->lineno, "method call, parameter count mismatch for call to '%s' in '%s'.", expr->method, ast_type_str(expr->object->expr_type));

	expr->expr_type = expr->bind_method->type;
}

void ast_bind_expressions(ast_methoddecl *method, ast_expr *expr)
{
	if(expr == NULL)
		return;

	switch(expr->type)
	{
		case INT_CONST:
			expr->expr_type.type=VAR_INT;
			break;
		case BOOL_CONST:
			expr->expr_type.type=VAR_BOOL;
			break;
		case VARNAME:
			expr->bind_var = symtab_find_var(expr->lineno, method, expr->id);
			expr->expr_type = expr->bind_var->type;
			break;
		case THIS_PTR:
			expr->expr_type = method->bind->class->type;
			break;
		case NOT_EXPR:
			expr->expr_type.type=VAR_BOOL;
			ast_bind_expressions(method, expr->expr);
			if(expr->expr->expr_type.type != VAR_BOOL)
				md_error(expr->lineno, "operator ! cannot be applied to '%s'.", ast_type_str(expr->expr->expr_type));
			break;
		case NEW_CLASS:
			expr->bind_class = symtab_find_class(expr->lineno, expr->id);
			expr->expr_type = expr->bind_class->type;
			break;
		case NEW_INT_ARRAY:
			ast_bind_expressions(method, expr->expr);
			if(expr->expr->expr_type.type != VAR_INT)
				md_error(expr->lineno, "new int[], found '%s', expected 'int'.", ast_type_str(expr->expr->expr_type));
			break;
		case ARRAY_LENGTH:
			ast_bind_expressions(method, expr->expr);
			if(expr->expr->expr_type.type != VAR_INT_ARRAY)
				md_error(expr->lineno, "array length, found '%s', expected 'int[]'.", ast_type_str(expr->expr->expr_type));
			expr->expr_type.type = VAR_INT;
			break;
		case ARRAY_INDEX:
			ast_bind_expressions(method, expr->array);
			if(expr->array->expr_type.type != VAR_INT_ARRAY)
				md_error(expr->lineno, "array, found '%s', expected 'int[]'.", ast_type_str(expr->array->expr_type));

			ast_bind_expressions(method, expr->array_index);
			if(expr->array_index->expr_type.type != VAR_INT)
				md_error(expr->lineno, "array index, found '%s', expected 'int'.", ast_type_str(expr->array_index->expr_type));

			expr->expr_type.type = VAR_INT;
			break;
		case METHOD_CALL:
			ast_bind_methodcall(method, expr);
			break;
		case BINOP:
			{
				ast_bind_expressions(method, expr->lhs);
				ast_bind_expressions(method, expr->rhs);

				ast_type tmp_type;

				switch(expr->oper)
				{
					case CONJ: // &&
						expr->expr_type.type = VAR_BOOL;
						printf("conj (%d)\n", expr->lineno);
						ast_expr_print(stdout, 1, expr->lhs);
						ast_expr_print(stdout, 1, expr->rhs);

						tmp_type.type = VAR_BOOL;
						ast_bind_typecheck(expr->lhs->lineno, tmp_type, expr->lhs->expr_type);
						ast_bind_typecheck(expr->rhs->lineno, tmp_type, expr->rhs->expr_type);
						break;
					case LESS: // <
						expr->expr_type.type = VAR_BOOL;

						tmp_type.type = VAR_INT;
						ast_bind_typecheck(expr->lhs->lineno, tmp_type, expr->lhs->expr_type);
						ast_bind_typecheck(expr->rhs->lineno, tmp_type, expr->rhs->expr_type);
						break;
					case PLUS: // +
						expr->expr_type.type = VAR_INT;

						tmp_type.type = VAR_INT;
						ast_bind_typecheck(expr->lhs->lineno, tmp_type, expr->lhs->expr_type);
						ast_bind_typecheck(expr->rhs->lineno, tmp_type, expr->rhs->expr_type);
						break;
					case MINUS: // -
						expr->expr_type.type = VAR_INT;

						tmp_type.type = VAR_INT;
						ast_bind_typecheck(expr->lhs->lineno, tmp_type, expr->lhs->expr_type);
						ast_bind_typecheck(expr->rhs->lineno, tmp_type, expr->rhs->expr_type);
						break;
					case MULT: // *
						expr->expr_type.type = VAR_INT;

						tmp_type.type = VAR_INT;
						ast_bind_typecheck(expr->lhs->lineno, tmp_type, expr->lhs->expr_type);
						ast_bind_typecheck(expr->rhs->lineno, tmp_type, expr->rhs->expr_type);
						break;
					default:
						md_error(expr->lineno, "operator not implemented.");
				}
			}
			break;
	}

	ast_bind_expressions(method, expr->next);
}

void ast_bind_statements(ast_methoddecl *method, ast_stmt *stmt)
{
	if(stmt == NULL)
		return;
	
	switch(stmt->type)
	{
		case BLOCK:
			ast_bind_statements(method, stmt->stmt_list);
			break;
		case IF_ELSE:
			ast_bind_expressions(method, stmt->cond);
			if(stmt->cond->expr_type.type != VAR_BOOL)
				md_error(stmt->cond->lineno, "if/else cannot be used with '%s'.", ast_type_str(stmt->cond->expr_type));
			ast_bind_statements(method, stmt->true_branch);
			ast_bind_statements(method, stmt->false_branch);
			break;
		case WHILE_STMT:
			ast_bind_expressions(method, stmt->cond);
			if(stmt->cond->expr_type.type != VAR_BOOL)
				md_error(stmt->cond->lineno, "while cannot be used with '%s'.", ast_type_str(stmt->cond->expr_type));
			ast_bind_statements(method, stmt->while_branch);
			break;
		case SYS_OUT:
			ast_bind_expressions(method, stmt->expr);
			if(stmt->expr->expr_type.type != VAR_INT)
				md_error(stmt->expr->lineno, "System.out.println() cannot be used with '%s'.", ast_type_str(stmt->cond->expr_type));
			break;
		case VAR_ASSIGN:
			// TODO: Look up id, bind variable, check types
			ast_bind_expressions(method, stmt->assign_expr);
			break;
		case ARRAY_ASSIGN:
			// TODO: Look up id, bind variable, check types
			ast_bind_expressions(method, stmt->assign_expr);
			ast_bind_expressions(method, stmt->array_index);
			break;
	}

	ast_bind_statements(method, stmt->next);
}

void ast_bind_methods(ast_methoddecl *methods)
{
	if(methods == NULL)
		return;

	ast_bind_statements(methods, methods->body);

	ast_bind_expressions(methods, methods->return_expr);

	if(methods->return_expr != NULL) // not main method
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
	ast_bind_methods(md_ast.main_class.method);

	// check other classes
	ast_bind_classes(md_ast.class_list);
}

