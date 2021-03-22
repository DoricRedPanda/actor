#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <stdio.h>
#include "list.h"
#include "poliz.h"

class Interpreter {
	FILE *file;
	int interpret(List<PolizItem*> &poliz);
public:
	Interpreter(FILE *file)
		: file(file) {}
	int run();
};


#endif
