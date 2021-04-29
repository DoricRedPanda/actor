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

	DataType(BaseType bt) : bt(bt), ptr(NULL), size(sizeof(int)) {}
	DataType(DataType *ptr) :
	    bt(POINTER), ptr(ptr), size(sizeof(intptr_t)) {}
	DataType(DataType *ptr, size_t size) :
	    bt(ARRAY), ptr(ptr), size(size) {}
	~DataType() { delete ptr; }

	size_t getSize();
};

struct Identifier {
	DataType *type;
	void *ptr;
	Identifier(DataType *type, void *ptr) : type(type), ptr(ptr) {}
	~Identifier() { delete type; }
	bool equals(BaseType bt) const;
};

struct LocalIdentifier {
	DataType *type;
	intptr_t ptr;
	LocalIdentifier(DataType *type, intptr_t ptr) :
	    type(type), ptr(ptr) {}
	~LocalIdentifier() { delete type; }
	bool equals(BaseType bt) const;
};

typedef Map<const char*, LocalIdentifier, strcmp> LocalSymTable;

class Parser {
	struct UndefinedLabel {
		const char *name;
		Label *label;
		int line;
		UndefinedLabel(const char name[], Label *label, int line):
		    name(name), label(label), line(line) {}
	};
	TokenList &list;
	Map<const char*, Identifier, strcmp> symbolTable;
	Stack<BaseType> typeStack;
	Stack<UndefinedLabel> labelStack;
	Token *token;
	TokenType tokenType;
	bool isSkippingNextGet;

	/*utility*/
	void get();
	void expect(TokenType type, const char errmsg[]);
	const char* expectIdentifier();
	/* syntax analyzer */
	void routine(Poliz *poliz);
	size_t processVar(LocalSymTable &localST);
	size_t processArguments(LocalSymTable &localST);
	void gvar(Poliz *poliz, BaseType baseType, const char *name);
	void subprogram(Poliz *poliz, BaseType baseType, const char *name);
	BaseType expectDataType();
	void body(Poliz *poliz, LocalSymTable &localST);
	void statement(Poliz *poliz, LocalSymTable &localST);
	void assignment(Poliz *poliz, LocalSymTable &localST);
	void keyword(Poliz *poliz, LocalSymTable &localST);
	void expression(Poliz *poliz, LocalSymTable &localST);
	void expressionArg(Poliz *poliz, LocalSymTable &localST);
	void handleIdentifierInExpression(Poliz *poliz,
	    LocalSymTable &localST, const char *name);
	void binaryOperation(Poliz *poliz, Stack<OpType> &opStack);
	void unaryOperation(Stack<OpType> &opStack);
	void statementGoto(Poliz *poliz);
	void identifierStatement(Poliz *poliz, LocalSymTable &localST,
	    const char *name);
	void returnStatement(Poliz *poliz, LocalSymTable &localST);
	void subprogramCall(Poliz *poliz, Identifier *identifier,
	    LocalSymTable &localST);
	void branching(Poliz *poliz, LocalSymTable &localST);
	void cycle(Poliz *poliz, LocalSymTable &localST);
	void writing(Poliz *poliz, LocalSymTable &localST);
	/* semantic analyzer */
	void declare(Poliz *poliz, DataType *type, const char *name);
	LocalIdentifier* declareLocalVariable(
	    DataType *type, LocalSymTable &localST,
	    size_t index, const char *name
	);
	void declareSubprogram(Poliz *poliz, DataType *type,
	    size_t varSize, const char *name);
	void declareLabel(Poliz *poliz, const char *name);
	void checkGlobalIdentifier(Poliz *poliz, Identifier *identifier,
	    LocalSymTable &localST);
	void checkLocalIdentifier(Poliz *poliz,
	    LocalIdentifier *identifier);
	void checkType();
	void checkType(BaseType expectedType);
	void insertLabels();
	void flushOperationStack(Poliz *poliz, Stack<OpType> &opStack);
	/* Poliz building */
	void insertInstruction(Poliz *poliz, OpType type);
public:
	Parser(TokenList *ptrList):
	    list(*ptrList), typeStack(STACK_SIZE),
	    labelStack(STACK_SIZE),
	    isSkippingNextGet(false) {}
	Poliz* analyze();
};


#endif
