#include "miniduke.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //chdir for -o

#include "jasmin.h"

extern int yyparse();

char *md_filename = NULL;

ast_program md_ast;
symtab_program md_symtab;

void md_error(int lineno, const char *error, ...)
{
	fprintf(stderr, "%s:%d: ", md_filename, lineno);
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
		if(md_filename != NULL)
			usage();

		md_filename = argv[i];
	}
}

char *basename(char *foo)
{
	char *out = foo;
	while(*foo != '\0')
	{
		if(*foo == '/' && foo[1] != '\0')
			out = foo+1;
		++foo;
	}
	--foo;

	while(foo > out)
	{
		if(*foo == '.')
		{
			*foo = '\0';
			break;
		}
		--foo;
	}

	return out;
}

int main(int argc, char *argv[])
{
	parse_args(argc, argv);

	if(md_filename == NULL)
		usage();

	yyin = fopen(md_filename, "r");
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

	// discard directory path and ".java" part
	md_filename = basename(md_filename);
	char out_filename[strlen(md_filename) + 16]; // +16 should be enough for all sane file endings
	sprintf(out_filename, "%s.syntax", md_filename);

	FILE *out_file = fopen(out_filename, "w");
	if(out_file)
	{
		ast_program_print(out_file, 0, &md_ast);
		fclose(out_file);
	}
	else
		fprintf(stderr, "warning: couldn't open AST file '%s' for writing.\n", out_filename);

	symtab_init();

	typecheck_symtab();

	ast_bind();

	sprintf(out_filename, "%s.symtab", md_filename);

	out_file = fopen(out_filename, "w");
	if(out_file)
	{
		symtab_print(out_file);
		fclose(out_file);
	}
	else
		fprintf(stderr, "warning: couldn't open symtab file '%s' for writing.\n", out_filename);

	jasmin_out();

	return 0;
}

