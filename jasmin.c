#include "jasmin.h"

#include "miniduke.h"

int indent = 0;
FILE *jasmin_file;

#define jasmin_print(...) \
	if(indent) fputs("\t", jasmin_file); \
	fprintf(jasmin_file, __VA_ARGS__); \
	fputs("\n", jasmin_file);

void jasmin_method(ast_methoddecl *method)
{
}

void jasmin_mainclass()
{
	jasmin_file = stderr;

	jasmin_print(".source %s.j", md_ast.main_class.id);
	jasmin_print(".class %s", md_ast.main_class.id);
	jasmin_print(".super java/lang/Object");
	jasmin_print(".method public <init>()V");
	indent = 1;
		jasmin_print("aload_0");
		jasmin_print("invokespecial java/lang/Object/<init>()V");
		jasmin_print("return");
	indent = 0;
	jasmin_print(".end method");

	//fclose();
}

void jasmin_classes(ast_classdecl *class)
{
	if(class == NULL)
		return;

	jasmin_file = stderr;

	jasmin_print(" "); //tmp

	jasmin_print(".source %s.j", class->id);
	jasmin_print(".class %s", class->id);
	jasmin_print(".super java/lang/Object");
	jasmin_print(".method public <init>()V");
	indent = 1;
		jasmin_print("aload_0");
		jasmin_print("invokespecial java/lang/Object/<init>()V");
		jasmin_print("return");
	indent = 0;
	jasmin_print(".end method");


	//fclose();

	jasmin_classes(class->next);
}

void jasmin_out()
{
	jasmin_mainclass();
	jasmin_classes(md_ast.class_list);
}

