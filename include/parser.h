#ifndef PARSER_H
#define PARSER_H
#include <string.h>
#include <stdio.h>

#include "lexem.h"
#include "list.h"
#include "map.h"
#include "stack.h"
#include "poliz.h"

typedef List<PolizItem*> Poliz;
typedef List<Lexem> LexemList;

struct Identifier {
	const DataType type;
	void *ptr;
	Identifier(DataType type, void *ptr)
		: type(type), ptr(ptr) {}
};

class Parser {
	LexemList &list;
	Map<const char *, Identifier, strcmp> symbolTable;
	Stack<DataType> typeStack;
	Stack<OpType> opStack;

	Lexem *lexem;
	LexemType lextype;
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
	Parser(LexemList &list)
		: list(list), typeStack(80), opStack(80) {}
	Poliz* analyze();
};

#endif
