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
typedef Node<PolizItem*> PolizItemNode;

struct Identifier {
	const BaseType type;
	void *ptr;
	Identifier(BaseType type, void *ptr)
		: type(type), ptr(ptr) {}
};

struct DataType {
	BaseType type;
	DataType *ptr;
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
	Stack<BaseType> typeStack;
	Stack<OpType> opStack;
	Stack<UndefinedLabel> labelStack;
	Token *token;
	TokenType tokenType;
	const char *idname;
	BaseType dtype;
	bool isSkippingNextGet;

	/*utility*/
	void get();
	void expect(TokenType type, const char errmsg[]);
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
	void branching(Poliz *poliz);
	void cycle(Poliz *poliz);
	void writing(Poliz *poliz);
	/* semantic analyzer */
	void declare(Poliz *poliz);
	void declareLabel(Poliz *poliz);
	void checkId(Poliz *poliz);
	void checkType();
	void checkAssignment(Poliz *poliz);
	void insertLabels();
	void flushOperationStack(Poliz *poliz);
	/* Poliz building */
	void insertInstruction(Poliz *poliz, OpType type);
public:
	Parser(TokenList *ptrList)
	    : list(*ptrList), typeStack(80),
	      opStack(80), labelStack(80),
	      isSkippingNextGet(false) {}
	Poliz* analyze();
};

#endif
