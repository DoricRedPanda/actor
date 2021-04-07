#ifndef PARSER_H
#define PARSER_H
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "list.h"
#include "map.h"
#include "stack.h"
#include "poliz.h"

typedef List<PolizItem*> Poliz;

struct Identifier {
	const DataType type;
	void *ptr;
	Identifier(DataType type, void *ptr)
		: type(type), ptr(ptr) {}
};

class Parser {
	TokenList &list;
	Map<const char *, Identifier, strcmp> symbolTable;
	Stack<DataType> typeStack;
	Stack<OpType> opStack;

	Token *token;
	TokenType tokenType;
	const char *idname;
	DataType dtype;

	void get();
	/* syntax analyzer */
	void var(Poliz *poliz);
	void gvar(Poliz *poliz);
	void function(Poliz *poliz);
	void dataType();
	void body(Poliz *poliz);
	void statement(Poliz *poliz);
	void keyword(Poliz *poliz);
	void expression(Poliz *poliz);
	void expressionArg(Poliz *poliz);
	/* semantic analyzer */
	void declare(Poliz *poliz);
	void checkId(Poliz *poliz);
	void checkType();
	void binaryOperation(Poliz *poliz);
	void unaryOperation();
	void assignment(Poliz *poliz);
	/* Poliz building */
	void insertInstruction(Poliz *poliz, OpType type);
public:
	Parser(TokenList *ptrList)
		: list(*ptrList), typeStack(80), opStack(80) {}
	Poliz* analyze();
};

#endif
