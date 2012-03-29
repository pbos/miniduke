#include "minijava.tab.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

extern int yyparse();

void md_error(int lineno, const char *error, ...)
{
	fprintf(stderr, "file:%d: ", lineno);
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
	yyin = fopen(argv[1], "r");
	if(yyin == NULL)
	{
		fprintf(stderr, "%s: ", argv[1]);
		perror("");
		return -1;
	}
	return yyparse();
}