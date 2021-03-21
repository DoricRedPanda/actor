#include <err.h>
#include <stdlib.h>
#include "parser.h"

void Parser::
get()
{
	lexem = list.next();
	lextype =  lexem->getType();
	if (lextype == IDENTIFIER)
		idname = lexem->getIdentifier();
	else if (lextype == DATA_TYPE)
		dtype = lexem->getDataType();
}

void Parser::
declare()
{
	int res;
	Identifier ident(dtype);
	res = symbolTable.insert(idname, ident);
	if (res < 0)
		errx(EXIT_FAILURE, "Identifier is already declared!");
}

void Parser::
checkId()
{
	Identifier *identifier = symbolTable.find(idname);
	if (identifier)
		typeStack.push(identifier->type);
	else
		errx(EXIT_FAILURE, "Not declared!");
}

void Parser::
checkAssign()
{
	if (typeStack.pop() != typeStack.pop())
		errx(EXIT_FAILURE, "Assigning wrong type!");
}

void Parser::
checkBinOp() //TODO redesign
{
	DataType foo, bar;
	bar = typeStack.pop();
	foo = typeStack.pop();
	if (foo == bar)
		typeStack.push(foo);
	else
		errx(EXIT_FAILURE, "Wrong type!");
}

void Parser::
gvar()
{
	for (;;) {
		if (lextype == COMMA) {
			declare();
			get();
			if (lextype != IDENTIFIER)
				errx(EXIT_FAILURE, "Identifier expected!");
			get();
			continue;
		}
		if (lextype == SEMICOLON)
			break;
		errx(EXIT_FAILURE, "gvar ';' expected!");
	}
	declare();
}

void Parser::
var()
{
	//TODO Syntax analysis for local variables
}

void Parser::
expression()
{
	bool isFirst = true;
	for (;;) {
		get();
		if (lextype == CONST_INT) {
			typeStack.push(INT);
		} else if (lextype == IDENTIFIER) {
			//TODO function call
			checkId();
		} else if (lextype == LPARENTHESIS) {
			expression();
			if (lextype != RPARENTHESIS)
				errx(EXIT_FAILURE, "')' expected!");
		} else if (lextype == OPERATOR) {
			//UNARY!
			continue;
		} else {
			errx(EXIT_FAILURE, "BAD EXPRESSION!");
		}
		if (!isFirst)
			checkBinOp();
		get();
		if (lextype != OPERATOR)
			break;
		isFirst = false;
	}
}

void Parser::
keyword()
{
	//TODO goto, if, while, read, write
}

void Parser::
statement()
{
	get();
	if (lextype == SEMICOLON)
		return;
	if (lextype == IDENTIFIER) {
		get();
		if (lextype != EQUALSIGN)
			errx(EXIT_FAILURE, "'=' expected!");
		checkId();
		expression();
		checkAssign();
		if (lextype != SEMICOLON)
			errx(EXIT_FAILURE, "';' expected!");
	} else if (lextype == STATEMENT) {
		keyword();
	} else if (lextype == BEGIN) {
		body();
	}

}

void Parser::
body()
{
	for (;;) {
		statement();
		if (lextype == END)
			break;
	}
}

void Parser::
function()
{
	get();
	if (lextype != RPARENTHESIS)
		errx(EXIT_FAILURE, "')' expected!");
	get();
	if (lextype == BEGIN)
		body();
	else
		errx(EXIT_FAILURE, "'{' expected!");
}

void Parser::
dataType()
{
	if (lextype != DATA_TYPE)
		errx(EXIT_FAILURE, "Data type expected!");
}

void Parser::
analyze()
{
	for (;;) {
		get();
		if (lextype == LEX_NULL)
			break;
		dataType();
		get();
		if (lextype != IDENTIFIER)
			errx(EXIT_FAILURE, "Identifier expected!");
		get();
		if (lextype == LPARENTHESIS)
			function();
		else
			gvar();
	}
}
