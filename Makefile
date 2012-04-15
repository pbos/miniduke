all: minijava.y minijava.l miniduke.c ast.c symtab.c
	bison --report=all -d minijava.y
	flex minijava.l
	gcc -g -Wall -o miniduke miniduke.c ast.c lex.yy.c minijava.tab.c symtab.c

