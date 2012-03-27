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

int main()
{
	return yyparse();
}
