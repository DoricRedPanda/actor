#include <err.h>
#include <stdlib.h>
#include "poliz.h"

void PolizExit::
eval(SStack &stack, Poliz *ptrPoliz)
{
	stack = stack;
	ptrPoliz->setPos(NULL);
#ifdef DEBUG
	fprintf(stderr, "exit\n");
#endif
}

void PolizOpGo::
eval(SStack &stack, Poliz *ptrPoliz)
{
	PolizItemNode *pos =
	    reinterpret_cast<PolizItemNode*>(stack.pop());
	ptrPoliz->setPos(pos);
	ptrPoliz->next();
#ifdef DEBUG
	fprintf(stderr, "goto %p\n", (void*) pos);
#endif
}

void PolizOpGoFalse::
eval(SStack &stack, Poliz *ptrPoliz)
{
	PolizItemNode *pos =
	    reinterpret_cast<PolizItemNode*>(stack.pop());
	intptr_t value = stack.pop();
	if (!value) {
		ptrPoliz->setPos(pos);
		ptrPoliz->next();
	}
#ifdef DEBUG
	fprintf(stderr, "goto %p if not %ld\n", (void*) pos, value);
#endif
}


void SubprogramCall::
eval(SStack &stack, Poliz *ptrPoliz)
{
	PolizItemNode *retAddress = ptrPoliz->getPos();
	PolizItemNode *subprogramAddress =
	    reinterpret_cast<PolizItemNode*>(stack.pop());
	ptrPoliz->setPos(subprogramAddress);
	stack.push(reinterpret_cast<intptr_t>(retAddress));
#ifdef DEBUG
	fprintf(stderr, "call %p\n", (void*) subprogramAddress);
#endif
}

void Subprogram::
eval(SStack &stack)
{
	stack.createFrame();
	stack.reserve(localVarCount);
#ifdef DEBUG
	fprintf(stderr, "Create stack frame. Old BP = %ld, New BP = %ld\n",
	    stack[0], stack.getBP());
	fprintf(stderr, "reserve %ld local variables\n", localVarCount);
#endif
}

void CallerRemovesArguments::
eval(SStack &stack)
{
	intptr_t retValue = stack.pop();
	stack.slice(argNum);
	stack.push(retValue);
}

void ProcedureReturn::
eval(SStack &stack, Poliz *ptrPoliz)
{
	PolizItemNode *pos;
	stack.destroyFrame();
	pos = reinterpret_cast<PolizItemNode*>(stack.pop());
	ptrPoliz->setPos(pos);
#ifdef DEBUG
	fprintf(stderr, "procedure ret\n");
	fprintf(stderr, "Restore BP %ld\n", stack.getBP());
#endif
}

void FunctionReturn::
eval(SStack &stack, Poliz *ptrPoliz)
{
	PolizItemNode *pos;
	intptr_t returnValue = stack.pop();
	stack.destroyFrame();
	pos = reinterpret_cast<PolizItemNode*>(stack.pop());
	ptrPoliz->setPos(pos);
	stack.push(returnValue);
#ifdef DEBUG
	fprintf(stderr, "ret %ld\n", returnValue);
	fprintf(stderr, "Restore BP %ld\n", stack.getBP());
#endif
}

void Inst_print::
eval(SStack &stack)
{
#ifdef DEBUG
	fprintf(stderr, "write:\n");
#endif
	intptr_t foo = stack.pop();
	printf("%ld\n", foo);
}

void Inst_AddrPlus::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	int *foo = reinterpret_cast<int*>(stack.pop());
	stack.push(reinterpret_cast<intptr_t>(foo + bar));
#ifdef DEBUG
	fprintf(stderr, "%p address+ %ld\n", (void*) foo, bar);
#endif
}

void Inst_mov::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	int *foo = reinterpret_cast<int*>(stack.pop());
	*foo = bar;
#ifdef DEBUG
	fprintf(stderr, "mov %p, %ld\n", (void*) foo, bar);
#endif
}

void Inst_movStack::
eval(SStack &stack)
{
	intptr_t value = stack.pop();
	intptr_t index = stack.pop();
	stack[index] = value;
#ifdef DEBUG
	fprintf(stderr, "stack_mov [%ld + %ld], %ld\n",
	    stack.getBP(), index, value);
#endif
}

void Inst_or::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo || bar);
#ifdef DEBUG
	fprintf(stderr, "%ld || %ld\n", foo, bar);
#endif
}

void Inst_and::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo && bar);
#ifdef DEBUG
	fprintf(stderr, "%ld && %ld\n", foo, bar);
#endif
}

void Inst_bitor::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo | bar);
#ifdef DEBUG
	fprintf(stderr, "%ld | %ld\n", foo, bar);
#endif
}

void Inst_xor::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo ^ bar);
#ifdef DEBUG
	fprintf(stderr, "%ld ^ %ld\n", foo, bar);
#endif
}

void Inst_bitand::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo & bar);
#ifdef DEBUG
	fprintf(stderr, "%ld & %ld\n", foo, bar);
#endif
}

void Inst_eq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo == bar);
#ifdef DEBUG
	fprintf(stderr, "%ld == %ld\n", foo, bar);
#endif
}

void Inst_neq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo != bar);
#ifdef DEBUG
	fprintf(stderr, "%ld != %ld\n", foo, bar);
#endif
}

void Inst_leq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo <= bar);
#ifdef DEBUG
	fprintf(stderr, "%ld <= %ld\n", foo, bar);
#endif
}

void Inst_lt::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo < bar);
#ifdef DEBUG
	fprintf(stderr, "%ld < %ld\n", foo, bar);
#endif
}

void Inst_geq::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo >= bar);
#ifdef DEBUG
	fprintf(stderr, "%ld >= %ld\n", foo, bar);
#endif
}

void Inst_gt::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo > bar);
#ifdef DEBUG
	fprintf(stderr, "%ld > %ld\n", foo, bar);
#endif
}

void Inst_shl::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo << bar);
#ifdef DEBUG
	fprintf(stderr, "%ld << %ld\n", foo, bar);
#endif
}

void Inst_shr::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo >> bar);
#ifdef DEBUG
	fprintf(stderr, "%ld >> %ld\n", foo, bar);
#endif
}

void Inst_add::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo + bar);
#ifdef DEBUG
	fprintf(stderr, "%ld + %ld\n", foo, bar);
#endif
}

void Inst_sub::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo - bar);
#ifdef DEBUG
	fprintf(stderr, "%ld - %ld\n", foo, bar);
#endif
}

void Inst_mul::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	stack.push(foo * bar);
#ifdef DEBUG
	fprintf(stderr, "%ld * %ld\n", foo, bar);
#endif
}

void Inst_div::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	if (!bar)
		errx(EXIT_FAILURE, "RT: division by zero");
	stack.push(foo / bar);
#ifdef DEBUG
	fprintf(stderr, "%ld / %ld\n", foo, bar);
#endif
}

void Inst_mod::
eval(SStack &stack)
{
	intptr_t bar = stack.pop();
	intptr_t foo = stack.pop();
	if (!bar)
		errx(EXIT_FAILURE, "RT: division by zero");
	stack.push(foo % bar);
#ifdef DEBUG
	fprintf(stderr, "%ld %% %ld\n", foo, bar);
#endif
}

void Inst_not::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(!foo);
#ifdef DEBUG
	fprintf(stderr, "! %ld\n", foo);
#endif
}

void Inst_bitnot::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(~foo);
#ifdef DEBUG
	fprintf(stderr, "~ %ld\n", foo);
#endif
}

void Inst_neg::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(-foo);
#ifdef DEBUG
	fprintf(stderr, "neg %ld\n", foo);
#endif
}

void Inst_dereference::
eval(SStack &stack)
{
	int *foo = reinterpret_cast<int*>(stack.pop());
	stack.push(*foo);
#ifdef DEBUG
	fprintf(stderr, "dereference [%p] = %d\n", (void*) foo, *foo);
#endif
}

void Inst_dereferenceStack::
eval(SStack &stack)
{
	intptr_t foo = stack.pop();
	stack.push(stack[foo]);
#ifdef DEBUG
	fprintf(stderr, "(stack dereference) [%ld + %ld] = %ld\n",
	    stack.getBP(), foo, stack[foo]);
#endif
}
