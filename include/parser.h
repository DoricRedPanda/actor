#ifndef PARSER_H
#define PARSER_H
#include <string.h>
#include <stdio.h>

#include "lexem.h"
#include "list.h"
#include "map.h"

class Identifier {
	void *dummy;
};

class Parser {
	List<Lexem> &list;
	Map<const char *, Identifier, strcmp> symbolTable;
	Lexem *lexem;
	void get() { lexem = list.next(); }
	/* states */
	void var();
	void gvar();
	void function();
	void dataType();
	void body();
	void statement();
	void keyword();
	void expression();
public:
	Parser(List<Lexem> &list)
		: list(list) {}
	void analyze();
};

#endif
