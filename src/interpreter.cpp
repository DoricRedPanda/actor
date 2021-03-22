#include <stdint.h>
#include "interpreter.h"
#include "stack.h"
#include "lexer.h"
#include "parser.h"

static int sstack_len = 2048;

int Interpreter::
run()
{
	Lexer lexer(file);
	List<Lexem> *list = lexer.analyze();
	Parser parser(*list);
	Poliz *poliz = parser.analyze();
	delete list;
	int exit_code = interpret(*poliz);
	delete poliz;
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
