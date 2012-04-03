all: minijava.y minijava.l
	bison --report=all -d minijava.y
	flex minijava.l
	gcc -g -Wall -o miniduke miniduke.c ast.c lex.yy.c minijava.tab.c

calc: calc.y calc.l
	bison -d calc.y
	flex calc.l
	gcc -Wall -o calc lex.yy.c calc.tab.c
