#ifndef IR_H
#define IR_H

#include <stdio.h>
#include <stdint.h>

struct ir_exp;

typedef struct {
	enum {
		ir_binop,
		ir_cmp,
		ir_label,
		ir_int32,
		ir_move,
		ir_return,
	} type;
	union {
		struct {
			enum {
				add,
				sub,
				mul,
			} op;
			struct ir_exp *false_branch, *true_branch;
			struct ir_exp *lhs, *rhs;
		} binop;
		struct {
			enum {
				conj,
				less
			} op;
			struct ir_exp *lhs, *rhs;
		} cmp;
		struct {
			const char *name;
			int i;
		} label;
		uint32_t int32;
		struct {
			uint32_t reg;
			uint32_t temp;
		} move;
	};
	struct ir_exp *next;
} ir_exp;

typedef struct {
	
} ir_func;

typedef struct {
	
} ir_program;

void ir_init();
void ir_print(FILE *file);

#endif

