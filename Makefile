all:
	bison -d minijava.y
	flex minijava.l
	gcc -Wall -o miniduke lex.yy.c minijava.tab.c

calc:
	bison -d calc.y
	flex calc.l
	gcc -Wall -o calc lex.yy.c calc.tab.c