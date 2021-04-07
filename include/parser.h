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
	struct UndefinedLabel {
		const char *id;
		Label *label;
		int line;
		UndefinedLabel(const char id[], Label *label, int line)
		    : id(id), label(label), line(line) {}
	};
	TokenList &list;
	Map<const char *, Identifier, strcmp> symbolTable;
	Stack<DataType> typeStack;
	Stack<OpType> opStack;
	Stack<UndefinedLabel> labelStack;
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
	void binaryOperation(Poliz *poliz);
	void unaryOperation();
	void statementGoto(Poliz *poliz);
	/* semantic analyzer */
	void declare(Poliz *poliz);
	void declareLabel(Poliz *poliz);
	void checkId(Poliz *poliz);
	void checkType();
	void checkAssignment(Poliz *poliz);
	void insertLabels();
	/* Poliz building */
	void insertInstruction(Poliz *poliz, OpType type);
public:
	Parser(TokenList *ptrList)
	    : list(*ptrList), typeStack(80),
	      opStack(80), labelStack(80) {}
	Poliz* analyze();
};

#endif
