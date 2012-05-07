all: minijava.y minijava.l *.c
	bison --report=all -d minijava.y
	flex minijava.l
	gcc -g -Wall -o miniduke *.c

