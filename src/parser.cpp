#include <err.h>
#include <stdlib.h>
#include "parser.h"


void Parser::
gvar()
{
	for (;;) {
		if (lexem->getType() == COMMA) {
			//INSERT
			get();
			if (lexem->getType() != IDENTIFIER)
				errx(EXIT_FAILURE, "Identifier expected!");
			get();
			continue;
		}
		if (lexem->getType() == SEMICOLON)
			break;
		errx(EXIT_FAILURE, "gvar ';' expected!");
	}
	//INSERT
}

void Parser::
var()
{
	//TODO Syntax for local variables
}

void Parser::
expression()
{
	for (;;) {
		get();
		if (lexem->getType() == CONST_INT) {
		} else if (lexem->getType() == IDENTIFIER) {
			//TODO function call
		} else if (lexem->getType() == LPARENTHESIS) {
			expression();
			if (lexem->getType() != RPARENTHESIS)
				errx(EXIT_FAILURE, "')' expected!");
		} else if (lexem->getType() == OPERATOR) {
			//UNARY!
			continue;
		} else {
			errx(EXIT_FAILURE, "BAD EXPRESSION!");
		}
		get();
		if (lexem->getType() != OPERATOR)
			break;
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
	if (lexem->getType() == SEMICOLON)
		return;
	if (lexem->getType() == IDENTIFIER) {
		get();
		if (lexem->getType() != EQUALSIGN)
			errx(EXIT_FAILURE, "'=' expected!");
		expression();
		if (lexem->getType() != SEMICOLON)
			errx(EXIT_FAILURE, "';' expected!");
	} else if (lexem->getType() == STATEMENT) {
		keyword();
	} else if (lexem->getType() == BEGIN) {
		body();
	}

}

void Parser::
body()
{
	for (;;) {
		statement();
		if (lexem->getType() == END)
			break;
	}
}

void Parser::
function()
{
	get();
	if (lexem->getType() != RPARENTHESIS)
		errx(EXIT_FAILURE, "')' expected!");
	get();
	if (lexem->getType() == BEGIN)
		body();
	else
		errx(EXIT_FAILURE, "'{' expected!");
}

void Parser::
dataType()
{
	if (lexem->getType() != DATA_TYPE)
		errx(EXIT_FAILURE, "Data type expected!");
}

void Parser::
analyze()
{
	for (;;) {
		get();
		if (lexem->getType() == LEX_NULL)
			break;
		dataType();
		get();
		if (lexem->getType() != IDENTIFIER)
			errx(EXIT_FAILURE, "Identifier expected!");
		get();
		if (lexem->getType() == LPARENTHESIS)
			function();
		else
			gvar();
	}
}
