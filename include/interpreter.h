#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <stdio.h>
#include "poliz.h"
#include "lexer.h"
#include "parser.h"

class Interpreter {
	FILE *file;
	LexemList *lexemList;
	Poliz *poliz;
	int interpret(List<PolizItem*> &poliz);
public:
	Interpreter(FILE *file)
		: file(file), lexemList(NULL), poliz(NULL) {}
	~Interpreter()
	{
		delete lexemList;
		delete poliz;
	}
	int run();
};


#endif
