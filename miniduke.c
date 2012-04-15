#include "miniduke.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern int yyparse();

const char *filename;

ast_program md_ast;
symtab_program md_symtab;

void md_error(int lineno, const char *error, ...)
{
	fprintf(stderr, "%s:%d: ", filename, lineno);
	va_list args;
	va_start (args, error);
	vfprintf (stderr, error, args);
	va_end (args); 
	fputs("\n", stderr);

	exit(-1);
}

extern FILE *yyin;

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fputs("usage: miniduke [file.java]\n", stderr);
		return -1;
	}
	filename = argv[1];
	yyin = fopen(filename, "r");
	if(yyin == NULL)
	{
		fprintf(stderr, "%s: ", argv[1]);
		perror("");
		return -1;
	}
	if(yyparse())
		return -1;

	char ast_filename[strlen(md_ast.main_class.id) + strlen(".syntax") + 1];
	sprintf(ast_filename, "%s.syntax", md_ast.main_class.id);

	FILE *ast_file = fopen(ast_filename, "w");
	if(ast_file)
	{
		ast_program_print(ast_file, 0, &md_ast);
	}
	else
		fprintf(stderr, "warning: couldn't open AST file '%s' for writing.\n", ast_filename);

	return 0;
}
