#ifndef MINIDUKE_H
#define MINIDUKE_H

#include "minijava.tab.h"
void md_error(int lineno, const char *error, ...);

extern ast_program md_ast;

#endif

