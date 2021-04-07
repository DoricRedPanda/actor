#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <stdio.h>
#include "poliz.h"
#include "lexer.h"
#include "parser.h"

class Interpreter {
	FILE *file;
	TokenList *ptrTokenList;
	Poliz *ptrPoliz;
	int interpret();
public:
	Interpreter(FILE *file)
		: file(file), ptrTokenList(NULL), ptrPoliz(NULL) {}
	~Interpreter()
	{
		delete ptrTokenList;
		delete ptrPoliz;
	}
	int run();
};


#endif
