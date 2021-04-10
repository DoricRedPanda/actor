#include <err.h>
#include <stdlib.h>
#include "poliz.h"

void PolizOpGo::
eval(SStack &stack, Poliz *ptrPoliz)
{
	Node<PolizItem*> *pos =
	    reinterpret_cast<Node<PolizItem*>*>(stack.pop());
	ptrPoliz->setPos(pos);
	ptrPoliz->next();
}

void PolizOpGoFalse::
eval(SStack &stack, Poliz *ptrPoliz)
{
	Node<PolizItem*> *pos =
	    reinterpret_cast<Node<PolizItem*>*>(stack.pop());
	intptr_t value = stack.pop();
	if (!value) {
		ptrPoliz->setPos(pos);
		ptrPoliz->next();
	}
}

void Inst_print::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	printf("%ld\n", foo);
}

void Inst_mov::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	int *foo = reinterpret_cast<int*>(stack.pop());
	*foo = bar;
}

void Inst_or::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo || bar);
}

void Inst_and::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo && bar);
}

void Inst_bitor::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo | bar);
}

void Inst_xor::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo ^ bar);
}

void Inst_bitand::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo & bar);
}

void Inst_eq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo == bar);
}

void Inst_neq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo != bar);
}

void Inst_leq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo <= bar);
}

void Inst_lt::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo < bar);
}

void Inst_geq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo >= bar);
}

void Inst_gt::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo > bar);
}

void Inst_shl::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo << bar);
}

void Inst_shr::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo >> bar);
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
	if (!bar)
		errx(EXIT_FAILURE, "RT: division by zero");
	foo /= bar;
	stack.push(foo);
}

void Inst_mod::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	if (!bar)
		errx(EXIT_FAILURE, "RT: division by zero");
	foo %= bar;
	stack.push(foo);
}

void Inst_not::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(!foo);
}

void Inst_bitnot::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(~foo);
}

void Inst_neg::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(-foo);
}

void Inst_dereference::
eval(SStack &stack)
{
	int *foo = reinterpret_cast<int*>(stack.pop());
	stack.push(*foo);
}
