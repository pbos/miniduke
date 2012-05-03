#ifndef MINIDUKE_H
#define MINIDUKE_H

#include "minijava.tab.h"
#include "symtab.h"
#include "typecheck.h"

void md_error(int lineno, const char *error, ...);

extern ast_program md_ast;
extern symtab_program md_symtab;

#endif

