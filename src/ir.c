#include "ir.h"

#include "miniduke.h"

void ir_init()
{
	ir_print(stderr);
}

void ir_print(FILE *file)
{
	fputs("yo\n", file);
}
