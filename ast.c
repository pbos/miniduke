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
			fprintf(file, "NEW_CLASS(%s)", expr->id);
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
			print_indent(file, indent_level);
			fprintf(file, "[\n");
				ast_expr_print(file, indent_level + 1, expr->array_index);
			print_indent(file, indent_level);
			fprintf(file, "]\n");
			break;
		case METHOD_CALL:
			fprintf(file, "METHOD CALL(%s)\n", expr->method);
			break;
		case BINOP:
			fprintf(file, "BINOP:\n");
				ast_expr_print(file, indent_level + 1, expr->lhs);
			print_indent(file, indent_level);
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
			fprintf(file, "%s\n", op_str);
				ast_expr_print(file, indent_level + 1, expr->rhs);
			break;
		default:
			fprintf(file, "!!! UNKNOWN EXPR :(((( !!!\n");
			break;
	}
	if(expr->next != NULL)
	{
		print_indent(file, indent_level);
		fputs(",\n", stderr);
		ast_expr_print(file, indent_level, expr->next);
	}
}

void ast_stmt_print(FILE *file, int indent_level, ast_stmt *stmt)
{
	print_indent(file, indent_level);

	switch(stmt->type)
	{
		case BLOCK:
			fputs("{\n", file);
			ast_stmt_print(file, indent_level+1, stmt->stmt_list);
			print_indent(file, indent_level);
			fputs("}\n", file);
			break;
		default:
			fprintf(file, "--- UNKNOWN STMT :(((( ---\n");
			break;
	}
	if(stmt->next != NULL)
	{
		print_indent(file, indent_level);
		fputs(",\n", stderr);
		ast_stmt_print(file, indent_level, stmt->next);
	}
}
