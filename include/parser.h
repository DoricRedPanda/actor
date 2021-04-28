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
	bool equals(BaseType bt) const;
};

struct LocalIdentifier {
	DataType *type;
	intptr_t ptr;
	LocalIdentifier(DataType *type, intptr_t ptr)
	    : type(type), ptr(ptr) {}
	~LocalIdentifier() { delete type; }
	bool equals(BaseType bt) const;
};

typedef Map<const char*, LocalIdentifier, strcmp> LocalSymbolTable;

class Parser {
	struct UndefinedLabel {
		const char *name;
		Label *label;
		int line;
		UndefinedLabel(const char name[], Label *label, int line)
		    : name(name), label(label), line(line) {}
	};
	TokenList &list;
	Map<const char*, Identifier, strcmp> symbolTable;
	Stack<BaseType> typeStack;
	Stack<UndefinedLabel> labelStack;
	Token *token;
	TokenType tokenType;
	/* FIXME get rid of dtype and hopefully idname */
	const char *idname;
	BaseType dtype;
	bool isSkippingNextGet;

	/*utility*/
	void get();
	void expect(TokenType type, const char errmsg[]);
	/* syntax analyzer */
	long var(LocalSymbolTable &localST);
	void gvar(Poliz *poliz);
	void subprogram(Poliz *poliz);
	void dataType();
	void body(Poliz *poliz, LocalSymbolTable &localST);
	void statement(Poliz *poliz, LocalSymbolTable &localST);
	void assignment(Poliz *poliz, LocalSymbolTable &localST);
	void keyword(Poliz *poliz, LocalSymbolTable &localST);
	void expression(Poliz *poliz, LocalSymbolTable &localST);
	void expressionArg(Poliz *poliz, LocalSymbolTable &localST);
	void binaryOperation(Poliz *poliz, Stack<OpType> &opStack);
	void unaryOperation(Stack<OpType> &opStack);
	void statementGoto(Poliz *poliz);
	void identifierStatement(Poliz *poliz, LocalSymbolTable &localST);
	void returnStatement(Poliz *poliz, LocalSymbolTable &localST);
	void subprogramCall(Poliz *poliz, Identifier *identifier);
	void branching(Poliz *poliz, LocalSymbolTable &localST);
	void cycle(Poliz *poliz, LocalSymbolTable &localST);
	void writing(Poliz *poliz, LocalSymbolTable &localST);
	/* semantic analyzer */
	void declare(Poliz *poliz, DataType *type);
	void declareLocalVariable(DataType *type, LocalSymbolTable &localST, size_t index);
	void declareLabel(Poliz *poliz);
	void checkIdentifier(Poliz *poliz, Identifier *identifier, LocalSymbolTable &localST);
	void checkLocalIdentifier(Poliz *poliz, LocalIdentifier *identifier);
	void checkType();
	void checkType(BaseType expectedType);
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
