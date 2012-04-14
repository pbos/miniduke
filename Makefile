all: minijava.y minijava.l
	bison --report=all -d minijava.y
	flex minijava.l
	gcc -g -Wall -o miniduke miniduke.c ast.c lex.yy.c minijava.tab.c

