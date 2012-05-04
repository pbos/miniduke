#include "miniduke.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //chdir for -o

extern int yyparse();

const char *filename = NULL;

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

	exit(1);
}

extern FILE *yyin;

char *out_dir = NULL;

void usage()
{
	fputs("usage: miniduke [file.java]\n", stderr);
	exit(1);
}

void parse_args(int argc, char *argv[])
{
	int i;
	for(i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-o"))
		{
			++i;
			if(i == argc)
			{
				fputs("usage: -o expects argument\n", stderr);
				exit(1);
			}

			out_dir = argv[i];
			continue;
		}
		if(filename != NULL)
			usage();

		filename = argv[i];
	}
}

int main(int argc, char *argv[])
{
	parse_args(argc, argv);

	if(filename == NULL)
		usage();

	yyin = fopen(filename, "r");
	if(yyin == NULL)
	{
		fprintf(stderr, "%s: ", argv[1]);
		perror("");
		return 1;
	}
	if(yyparse())
		return 1;

	if(out_dir != NULL)
		chdir(out_dir);

	char ast_filename[strlen(md_ast.main_class.id) + strlen(".syntax") + 1];
	sprintf(ast_filename, "%s.syntax", md_ast.main_class.id);

	FILE *ast_file = fopen(ast_filename, "w");
	if(ast_file)
	{
		ast_program_print(ast_file, 0, &md_ast);
	}
	else
		fprintf(stderr, "warning: couldn't open AST file '%s' for writing.\n", ast_filename);

	symtab_init();

	typecheck_symtab();

	symtab_print(stdout);

	return 0;
}

