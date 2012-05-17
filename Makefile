all: src/minijava.y src/minijava.l src/*.c
	bison --report=all --report-file=build/minijava.output -d src/minijava.y --defines=build/minijava.tab.h -o build/minijava.tab.c
	flex -o build/lex.yy.c src/minijava.l
	gcc -g -Wall -o miniduke -I src/ src/*.c -I build/ build/*.c

