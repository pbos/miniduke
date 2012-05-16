#include "jasmin.h"

#include "miniduke.h"

int indent = 0;
FILE *jasmin_file;
int jasmin_in_main = 0;

int label_count = 0;

#define jasmin_print(...) fprintf(jasmin_file, __VA_ARGS__)

#define jasmin_println(...) do { \
	if(indent) fputs("\t", jasmin_file); \
	fprintf(jasmin_file, __VA_ARGS__); \
	fputs("\n", jasmin_file); } while(0)

#define LABEL(type, n) fprintf(jasmin_file, type "%d:\n", n)

void jasmin_print_type(ast_type type)
{
	switch(type.type)
	{
		case VAR_BOOL:
			jasmin_print("Z");
			break;
		case VAR_INT:
			jasmin_print("I");
			break;
		case VAR_INT_ARRAY:
			jasmin_print("[I");
			break;
		case VAR_STRING_ARRAY:
			jasmin_print("[Ljava/lang/String;");
			break;
		case VAR_CLASS:
			jasmin_print("L%s;", type.classname);
			break;
		default:
			md_error(-1, "!! unsupported type, during jasmin generation, this should never happen");
	}
}

void jasmin_print_params(symtab_var *params)
{
	for(; params != NULL; params = params->next)
		jasmin_print_type(params->type);
}

void jasmin_expr(ast_expr *expr)
{
	switch(expr->type)
	{
		case INT_CONST:
			jasmin_println("ldc %d", expr->int_const);
			break;
		case BOOL_CONST:
			jasmin_println(expr->bool_const ? "iconst_1" : "iconst_0");
			break;
		case VARNAME:
		{
			switch(expr->bind_var->type.type)
			{
				case VAR_INT:
				case VAR_BOOL:
					jasmin_println("iload %d ; load %s", expr->bind_var->var_reg, expr->bind_var->id);
					break;
				case VAR_INT_ARRAY:
				case VAR_CLASS:
					jasmin_println("aload %d ; load %s", expr->bind_var->var_reg, expr->bind_var->id);
					break;
				default:
					md_error(-1, "!! unsupported VARNAME type during jasmin generation, this should never happen");
			}
			break;
		}
		case THIS_PTR:
		{
			jasmin_println("aload 0 ; load <this>");
			break;
		}
		case NOT_EXPR:
			jasmin_expr(expr->expr);
			jasmin_println("iconst_1");
			jasmin_println("ixor"); // 0=>1, 1=>0
			break;
		case NEW_CLASS:
			jasmin_println("new %s", expr->id);
			jasmin_println("dup");
			jasmin_println("invokespecial %s/<init>()V", expr->id);
			break;
		case NEW_INT_ARRAY:
			jasmin_expr(expr->expr);
			jasmin_println("newarray int");
			break;
		case ARRAY_LENGTH:
			jasmin_expr(expr->expr);
			jasmin_print("arraylength");
			break;
		case ARRAY_INDEX:
			jasmin_expr(expr->array);
			jasmin_expr(expr->array_index);
			jasmin_println("iaload");
			break;

		case METHOD_CALL:
		{
			jasmin_expr(expr->object);

			ast_expr *params;
			for(params = expr->exp_list; params != NULL; params = params->next)
				jasmin_expr(params);

			jasmin_print("\tinvokevirtual %s/%s(", expr->object->expr_type.classname, expr->method);
			jasmin_print_params(expr->bind_method->params);
			jasmin_print(")");
			jasmin_print_type(expr->bind_method->type);
			jasmin_print("\n");
		}
			break;
		case BINOP:
		{
			switch(expr->oper)
			{
				case CONJ:
				{
					int label = label_count++;

					jasmin_expr(expr->lhs);
					jasmin_println("ifeq FALSE%d", label); // abort if false
					jasmin_expr(expr->rhs);
					jasmin_println("ifeq FALSE%d", label); // false
					jasmin_println("iconst_1");
					jasmin_println("goto END%d", label);
					LABEL("FALSE", label);
					jasmin_println("iconst_0");
					LABEL("END", label);
				}
					break;
				case LESS:
				{
					int label = label_count++;
					jasmin_expr(expr->lhs);
					jasmin_expr(expr->rhs);
					jasmin_println("if_icmplt TRUE%d", label);
					jasmin_println("iconst_0");
					jasmin_println("goto END%d", label);
					LABEL("TRUE", label);
					jasmin_println("iconst_1");
					LABEL("END", label);
				}
					break;
				case PLUS:
					jasmin_expr(expr->lhs);
					jasmin_expr(expr->rhs);
					jasmin_println("iadd");
					break;
				case MINUS:
					jasmin_expr(expr->lhs);
					jasmin_expr(expr->rhs);
					jasmin_println("isub");
					break;
				case MULT:
					jasmin_expr(expr->lhs);
					jasmin_expr(expr->rhs);
					jasmin_println("imul");
					break;
				default:
					md_error(-1, "unknown jasmin binop");
			}
			break;
		}
	}
}

void jasmin_statements(ast_stmt *stmt)
{
	if(stmt == NULL)
		return;	

	switch(stmt->type)
	{
		case BLOCK:
			jasmin_statements(stmt->stmt_list);
			break;
		case IF_ELSE:
		{
			int if_label = label_count++;

			jasmin_expr(stmt->cond);
			jasmin_println("ifeq FALSE%d", if_label);
			jasmin_statements(stmt->true_branch);
			jasmin_println("goto END%d", if_label);
			LABEL("FALSE", if_label);
			jasmin_statements(stmt->false_branch);
			LABEL("END", if_label);
			jasmin_print("\n");
			break;
		}
		case WHILE_STMT:
		{
			int loop_label = label_count++;
			LABEL("LOOP", loop_label);
			jasmin_expr(stmt->cond);
			jasmin_println("ifeq END%d", loop_label);
			jasmin_statements(stmt->true_branch);
			jasmin_println("goto LOOP%d", loop_label);
			LABEL("END", loop_label);
			jasmin_print("\n");
			break;
		}
		case SYS_OUT:
			jasmin_println("getstatic java/lang/System/out Ljava/io/PrintStream;");
			jasmin_expr(stmt->expr);
			jasmin_println("invokevirtual java/io/PrintStream/println(I)V");
			jasmin_print("\n");
			break;
		case VAR_ASSIGN:
		{
			jasmin_expr(stmt->assign_expr);
			switch(stmt->bind->type.type)
			{
				case VAR_BOOL:
				case VAR_INT:
					jasmin_println("istore %d ; store %s", stmt->bind->var_reg, stmt->bind->id);
					break;
				case VAR_INT_ARRAY:
				case VAR_CLASS:
					jasmin_println("astore %d ; store %s", stmt->bind->var_reg, stmt->bind->id);
					break;
				default:
					md_error(-1, "!! unsupported VAR_ASSIGN type during jasmin generation, this should never happen");
			}
			jasmin_print("\n");
			break;
		}
		case ARRAY_ASSIGN:
			jasmin_println("aload %d ; load array %s", stmt->bind->var_reg, stmt->bind->id);
			jasmin_expr(stmt->array_index);
			jasmin_expr(stmt->assign_expr);
			// iastore
			jasmin_println("iastore");
			jasmin_print("\n");
			break;
	}

	jasmin_statements(stmt->next);
}

void jasmin_locals(ast_methoddecl *method)
{
	ast_vardecl *var;

	int i = 0;
	if(!jasmin_in_main) // count <this>
		++i;
	for(var = method->params; var != NULL; var = var->next)
		++i;
	for(var = method->var_decl; var != NULL; var = var->next)
		++i;

	jasmin_println(".limit locals %d", i);

	i = 0;
	if(!jasmin_in_main)
		jasmin_println(".var %d is <this> L%s;", i++, method->bind->class->id);
	for(var = method->params; var != NULL; var = var->next, ++i)
	{
		var->bind->var_reg = i;
		jasmin_print("\t.var %d is %s ", i, var->id);
		jasmin_print_type(var->type);
		jasmin_print("\n");
	}
	for(var = method->var_decl; var != NULL; var = var->next, ++i)
	{
		var->bind->var_reg = i;
		jasmin_print("\t.var %d is %s ", i, var->id);
		jasmin_print_type(var->type);
		jasmin_print("\n");
	}
	fputs("\n", jasmin_file);
}

void jasmin_stacksize(ast_methoddecl *method)
{
	jasmin_println(".limit stack ?");
}

void jasmin_method_body(ast_methoddecl *method)
{
	indent = 1;
	jasmin_stacksize(method);
	jasmin_locals(method);
	jasmin_statements(method->body);
	if(method->return_expr != NULL)
	{
		jasmin_expr(method->return_expr);
		switch(method->type.type)
		{
			case VAR_INT:
			case VAR_BOOL:
				jasmin_println("ireturn");
				break;
			case VAR_INT_ARRAY:
			case VAR_CLASS:
				jasmin_println("areturn");
				break;
			default:
				md_error(-1, "unknown type in return expr during jasmin generation, this should never happen");
		}
	}
	else
		jasmin_println("return");

	indent = 0;
}

void jasmin_methods(ast_methoddecl *method)
{
	if(method == NULL)
		return;
	jasmin_print("\n.method public %s(", method->id);
	jasmin_print_params(method->bind->params);
	jasmin_print(")");
	jasmin_print_type(method->type);
	jasmin_print("\n");
	jasmin_method_body(method);
	jasmin_println(".end method");

	jasmin_methods(method->next);
}

void jasmin_mainclass()
{
	jasmin_file = stderr;

	jasmin_println(".source %s.j", md_ast.main_class.id);
	jasmin_println(".class %s", md_ast.main_class.id);
	jasmin_println(".super java/lang/Object");
	jasmin_println("\n.method public <init>()V");
	indent = 1;
		jasmin_println("aload_0");
		jasmin_println("invokespecial java/lang/Object/<init>()V");
		jasmin_println("return");
	indent = 0;
	jasmin_println(".end method");

	jasmin_println("\n.method public static main([Ljava/lang/String;)V");
		jasmin_in_main = 1;
		jasmin_method_body(md_ast.main_class.method);
		jasmin_in_main = 0;
	jasmin_println(".end method");

	//fclose();
}

void jasmin_classes(ast_classdecl *class)
{
	if(class == NULL)
		return;

	jasmin_file = stderr;

	jasmin_println(" ---"); //tmp

	label_count = 0;

	jasmin_println(".source %s.j", class->id);
	jasmin_println(".class %s", class->id);
	jasmin_println(".super java/lang/Object");
	jasmin_println("\n.method public <init>()V");
	indent = 1;
		jasmin_println("aload_0");
		jasmin_println("invokespecial java/lang/Object/<init>()V");
		jasmin_println("return");
	indent = 0;
	jasmin_println(".end method");

	jasmin_methods(class->methods);

	//fclose();

	jasmin_classes(class->next);
}

void jasmin_out()
{
	jasmin_mainclass();
	jasmin_classes(md_ast.class_list);
}

