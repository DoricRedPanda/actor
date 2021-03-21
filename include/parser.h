#ifndef PARSER_H
#define PARSER_H
#include <string.h>
#include <stdio.h>

#include "lexem.h"
#include "list.h"
#include "map.h"
#include "stack.h"

struct Identifier {
	const DataType type;
	void *ptr;
	Identifier(DataType type)
		: type(type) {}
};

class Parser {
	List<Lexem> &list;
	Map<const char *, Identifier, strcmp> symbolTable;
	Stack<DataType> typeStack;
	Lexem *lexem;
	LexemType lextype;
	const char *idname;
	DataType dtype;
	void get();
	/* states */
	void var();
	void gvar();
	void function();
	void dataType();
	void body();
	void statement();
	void keyword();
	void expression();
	/* semantic analyzer */
	void declare();
	void checkId();
	void checkBinOp();
	void checkAssign();
public:
	Parser(List<Lexem> &list)
		: list(list), typeStack(80) {}
	void analyze();
	~Parser()
	{
		printf("stack %d\n", typeStack.getLength());
	}
};

#endif
