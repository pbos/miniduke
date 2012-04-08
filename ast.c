#include "ast.h"
#include "minijava.tab.h"

void print_indent(FILE *file, int indent_level)
{
	int i;
	for(i = 0; i < indent_level; ++i)
		fputs("  ", file);
}

void ast_expr_print(FILE *file, int indent_level, ast_expr *expr)
{
	if(expr == NULL)
		return; // If ExpList is empty, print nothing
	print_indent(file, indent_level);

	char *op_str;

	switch(expr->type)
	{
		case INT_CONST:
			fprintf(file, "INT_CONST(%d)\n", expr->int_const);
			break;
		case BOOL_CONST:
			fprintf(file, "BOOL_CONST(%s)\n", expr->bool_const ? "true" : "false");
			break;
		case VARNAME:
			fprintf(file, "VARNAME(%s)\n", expr->id);
			break;
		case THIS_PTR:
			fprintf(file, "THIS_PTR\n");
			break;
		case NOT_EXPR:
			fprintf(file, "NOT_EXPR:\n");
				ast_expr_print(file, indent_level + 1, expr->expr);
			break;
		case NEW_CLASS:
			fprintf(file, "NEW_CLASS(%s)\n", expr->id);
			break;
		case NEW_INT_ARRAY:
			fprintf(file, "NEW_INT_ARRAY[\n");
				ast_expr_print(file, indent_level + 1, expr->expr);
			print_indent(file, indent_level);
			fprintf(file, "]\n");
			break;
		case ARRAY_LENGTH:
			fprintf(file, "ARRAY_LENGTH:\n");
				ast_expr_print(file, indent_level + 1, expr->expr);
			break;
		case ARRAY_INDEX:
			fprintf(file, "ARRAY_INDEX:\n");
				ast_expr_print(file, indent_level + 1, expr->array);
			print_indent(file, indent_level+1);
			fprintf(file, "[\n");
				ast_expr_print(file, indent_level + 1, expr->array_index);
			print_indent(file, indent_level+1);
			fprintf(file, "]\n");
			break;
		case METHOD_CALL:
			fprintf(file, "METHOD_CALL(%s):\n", expr->method);
			print_indent(file, indent_level + 1);
			fprintf(file, "OBJECT:\n");
				ast_expr_print(file, indent_level + 2, expr->object);
			print_indent(file, indent_level + 1);
			fprintf(file, "EXP_LIST:\n");
				ast_expr_print(file, indent_level + 2, expr->exp_list);
			break;
		case BINOP:
			switch(expr->oper) {
				case CONJ:
					op_str = "CONJ";
					break;
				case LESS:
					op_str = "LESS";
					break;
				case PLUS:
					op_str = "PLUS";
					break;
				case MINUS:
					op_str = "MINUS";
					break;
				case MULT:
					op_str = "MULT";
					break;
				default:
					op_str = "--- UNKNOWN OPERAND ---";
					break;
			}
			fprintf(file, "BINOP(%s):\n", op_str);
			print_indent(file, indent_level + 1);
			fprintf(file, "LHS:\n");
				ast_expr_print(file, indent_level + 2, expr->lhs);
			print_indent(file, indent_level + 1);
			fprintf(file, "RHS:\n");
				ast_expr_print(file, indent_level + 2, expr->rhs);
			break;
		default:
			fprintf(file, "!!! UNKNOWN EXPR :(((( !!!\n");
			break;
	}
	if(expr->next != NULL) // ExpList
		ast_expr_print(file, indent_level, expr->next);
}

void ast_stmt_print(FILE *file, int indent_level, ast_stmt *stmt)
{
	if(stmt == NULL)
		return; // If StmtList is empty, print nothing.
	print_indent(file, indent_level);

	switch(stmt->type)
	{
		case BLOCK:
			fputs("BLOCK:\n", file);
				ast_stmt_print(file, indent_level+1, stmt->stmt_list);
			break;
		case IF_ELSE:
			fprintf(file, "IF_ELSE:\n");
			print_indent(file, indent_level + 1);
			fprintf(file, "COND:\n");
				ast_expr_print(file, indent_level+2, stmt->cond);
			print_indent(file, indent_level + 1);
			fprintf(file, "TRUE:\n");
				ast_stmt_print(file, indent_level+2, stmt->true_branch);
			print_indent(file, indent_level + 1);
			fprintf(file, "FALSE:\n");
				ast_stmt_print(file, indent_level+2, stmt->false_branch);
			break;
		case WHILE_STMT:
			fprintf(file, "WHILE_STMT:\n");
			print_indent(file, indent_level + 1);
			fprintf(file, "COND:\n");
				ast_expr_print(file, indent_level+2, stmt->cond);
			print_indent(file, indent_level + 1);
			fprintf(file, "WHILE_TRUE:\n");
				ast_stmt_print(file, indent_level+2, stmt->while_branch);
			break;
		case SYS_OUT:
			fprintf(file, "SYS_OUT:\n");
				ast_expr_print(file, indent_level + 1, stmt->expr);
			break;
		case VAR_ASSIGN:
			fprintf(file, "VAR_ASSIGN(%s):\n", stmt->id);
			print_indent(file, indent_level + 1);
			fprintf(file, "EXPR:\n");
				ast_expr_print(file, indent_level + 2, stmt->assign_expr);
			break;
		case ARRAY_ASSIGN:
			fprintf(file, "ARRAY_ASSIGN(%s):\n", stmt->id);
			print_indent(file, indent_level + 1);
			fprintf(file, "INDEX:\n");
				ast_expr_print(file, indent_level + 2, stmt->array_index);
			print_indent(file, indent_level + 1);
			fprintf(file, "EXPR:\n");
				ast_expr_print(file, indent_level + 2, stmt->assign_expr);
			break;
		default:
			fprintf(file, "--- UNKNOWN STMT :(((( ---\n");
			break;
	}
	if(stmt->next != NULL) // StmtList
		ast_stmt_print(file, indent_level, stmt->next);
}

const char *ast_type_str(ast_type type)
{
	switch (type.type)
	{
		case VAR_BOOL:
			return "boolean";
		case VAR_INT:
			return "int";
		case VAR_INT_ARRAY:
			return "int[]";
		case VAR_CLASS:
			return type.classname;
		default:
			return "-- UNKNOWN TYPE --";
	}
}

void ast_vardecl_print(FILE *file, int indent_level, ast_vardecl *decl)
{
	if(decl == NULL)
		return; // If VarList is empty, print nothing.

	print_indent(file, indent_level);
	fprintf(file, "%s : %s\n", decl->id, ast_type_str(decl->type));

	if(decl->next != NULL)
		ast_vardecl_print(file, indent_level, decl->next);
}

void ast_method_print(FILE *file, int indent_level, ast_methoddecl *method)
{
	if(method == NULL)
		return; // If MethodList is empty, print nothing.

	print_indent(file, indent_level);
	fprintf(file, "METHOD_DECL(%s : %s):\n", method->id, ast_type_str(method->type));
	print_indent(file, indent_level + 1);
	fprintf(file, "PARAMETERS:\n");
		ast_vardecl_print(file, indent_level + 2, method->params);
	print_indent(file, indent_level + 1);
	fprintf(file, "VAR_DECL:\n");
		ast_vardecl_print(file, indent_level + 2, method->var_decl);
	print_indent(file, indent_level + 1);
	fprintf(file, "BODY:\n");
		ast_stmt_print(file, indent_level + 2, method->body);
	print_indent(file, indent_level + 1);
	fprintf(file, "RETURN:\n");
		ast_expr_print(file, indent_level + 2, method->return_expr);

	if(method->next != NULL) // StmtList
		ast_method_print(file, indent_level, method->next);
}

void ast_class_print(FILE *file, int indent_level, ast_classdecl *class)
{
	if(class == NULL)
		return; // If MethodList is empty, print nothing.

	print_indent(file, indent_level);
	fprintf(file, "CLASS_DECL(%s):\n", class->id);
	print_indent(file, indent_level + 1);
	fprintf(file, "FIELDS:\n");
		ast_vardecl_print(file, indent_level + 2, class->fields);
	print_indent(file, indent_level + 1);
	fprintf(file, "METHODS:\n");
		ast_method_print(file, indent_level + 2, class->methods);

	if(class->next != NULL) // StmtList
		ast_class_print(file, indent_level, class->next);
}

void ast_main_print(FILE *file, int indent_level, ast_mainclass *main_class)
{
	print_indent(file, indent_level);
	fprintf(file, "MAIN_CLASS(%s):\n", main_class->id);
	print_indent(file, indent_level + 1);
	fprintf(file, "VAR_DECL:\n");
		ast_vardecl_print(file, indent_level + 2, main_class->main_vars);
	print_indent(file, indent_level + 1);
	fprintf(file, "BODY:\n");
		ast_stmt_print(file, indent_level + 2, main_class->main_body);
}

void ast_program_print(FILE *file, int indent_level, ast_program *program)
{
	print_indent(file, indent_level);
	fprintf(file, "PROGRAM:\n");
		ast_main_print(file, indent_level + 1, &program->main_class);
	print_indent(file, indent_level + 1);
	fprintf(file, "CLASSES:\n");
		ast_class_print(file, indent_level + 2, program->class_list);
}

