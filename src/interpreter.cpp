#include <stdint.h>
#include "interpreter.h"
#include "stack.h"

static int sstack_len = 2048;

int Interpreter::
run()
{
	ptrTokenList = Lexer(file).analyze();
	ptrPoliz = Parser(ptrTokenList).analyze();
	int exit_code = interpret();
	return exit_code;
}

int Interpreter::
interpret()
{
	SStack stack(sstack_len);
	PolizItem **ptr = ptrPoliz->next();
#ifdef DEBUG
	fprintf(stderr, "Interpretation:\n");
#endif
	while (!ptrPoliz->isEnding()) {
		(*ptr)->eval(stack, ptrPoliz);
		ptr = ptrPoliz->next();
	}
	return stack.pop();
}
