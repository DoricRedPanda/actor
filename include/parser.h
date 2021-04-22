#ifndef PARSER_H
#define PARSER_H
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "list.h"
#include "map.h"
#include "stack.h"
#include "poliz.h"

#define STACK_SIZE 32

typedef List<PolizItem*> Poliz;

struct DataType {
	BaseType bt;
	DataType *ptr;
	size_t size;

	DataType(BaseType bt)
	    : bt(bt), ptr(NULL), size(sizeof(int)) {}
	DataType(DataType *ptr)
	    : bt(POINTER), ptr(ptr), size(sizeof(intptr_t)) {}
	DataType(DataType *ptr, size_t size)
	    : bt(ARRAY), ptr(ptr), size(size) {}
	~DataType() { delete ptr; }

	size_t getSize();
};

struct Identifier {
	DataType *type;
	void *ptr;
	Identifier(DataType *type, void *ptr)
	    : type(type), ptr(ptr) {}
	~Identifier() { delete type; }
	bool checkType(BaseType bt) const;
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
	Map<const char*, Identifier, strcmp> symbolTable;
	Stack<BaseType> typeStack;
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
	void assignment(Poliz *poliz);
	void keyword(Poliz *poliz);
	void expression(Poliz *poliz);
	void expressionArg(Poliz *poliz);
	void binaryOperation(Poliz *poliz, Stack<OpType> &opStack);
	void unaryOperation(Stack<OpType> &opStack);
	void statementGoto(Poliz *poliz);
	void branching(Poliz *poliz);
	void cycle(Poliz *poliz);
	void writing(Poliz *poliz);
	/* semantic analyzer */
	void declare(Poliz *poliz, DataType *type);
	void declareLabel(Poliz *poliz);
	void checkId(Poliz *poliz);
	void checkType();
	void checkAssignment(Poliz *poliz);
	void insertLabels();
	void flushOperationStack(Poliz *poliz, Stack<OpType> &opStack);
	/* Poliz building */
	void insertInstruction(Poliz *poliz, OpType type);
public:
	Parser(TokenList *ptrList)
	    : list(*ptrList), typeStack(STACK_SIZE),
	      labelStack(STACK_SIZE),
	      isSkippingNextGet(false) {}
	Poliz* analyze();
};

#endif
