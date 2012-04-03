#include "ast.h"

void print_indent(FILE *file, int indent_level)
{
	int i;
	for(i = 0; i < indent_level; ++i)
		fputs("  ", file);
}

void ast_expr_print(FILE *file, int indent_level, ast_expr *expr)
{
	print_indent(file, indent_level);

	switch(expr->type)
	{
		case INT_CONST:
			fprintf(file, "INT_CONST(%d)\n", expr->int_const);
			break;
		case METHOD_CALL:
			fprintf(file, "METHOD CALL(%s)\n", expr->method);
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
