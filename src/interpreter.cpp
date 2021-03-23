#include <stdint.h>
#include "interpreter.h"
#include "stack.h"

static int sstack_len = 2048;

int Interpreter::
run()
{
	lexemList = Lexer(file).analyze();
	poliz = Parser(*lexemList).analyze();
	int exit_code = interpret(*poliz);
	return exit_code;
}

int Interpreter::
interpret(List<PolizItem*> &poliz)
{
	Stack<intptr_t> stack(sstack_len);
	PolizItem **ptr;
	for (;;) {
		ptr = poliz.next();
		if (!ptr)
			break;
		(*ptr)->eval(stack);
	}
	return 0; //TODO implement error handling
}
