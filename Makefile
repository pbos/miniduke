all: src/minijava.y src/minijava.l src/*.c
	mkdir -p src/gen/
	bison --report=all --report-file=src/gen/minijava.output -d src/minijava.y --defines=src/gen/minijava.tab.h -o src/gen/minijava.tab.c
	flex -o src/gen/lex.yy.c src/minijava.l
	gcc -g -Wall -o miniduke -I src/ src/*.c -I src/gen/ src/gen/*.c

