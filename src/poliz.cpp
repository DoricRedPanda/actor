#include "poliz.h"

void Inst_mov::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	printf("%ld\n", bar);
	int *foo = (int*) stack.pop();
	*foo = bar;
}

void Inst_add::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	foo += bar;
	stack.push(foo);
}

void Inst_sub::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	foo -= bar;
	stack.push(foo);
}

void Inst_mul::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	foo *= bar;
	stack.push(foo);
}

void Inst_div::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	foo /= bar;
	stack.push(foo);
}

void Inst_dereference::
eval(SStack &stack)
{
	int *foo = (int*) stack.pop(); //TODO redesign
	stack.push(*foo);
}
