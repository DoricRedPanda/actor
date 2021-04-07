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
	PolizItem **ptr;
	for (;;) {
		ptr = ptrPoliz->next();
		if (!ptr)
			break;
		(*ptr)->eval(stack);
	}
	return 0; //TODO implement error handling
}
