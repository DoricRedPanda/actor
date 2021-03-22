#include <err.h>
#include <stdlib.h>
#include "parser.h"

static const int precedence[] = {
	12,
	11,
	10,
	9,
	8,
	7, 7,
	6, 6, 6, 6,
	5, 5,
	4, 4,
	3, 3, 3,
	2, 2
};

void Parser::
get()
{
	lexem = list.next();
	lextype =  lexem->getType();
	if (lextype == IDENTIFIER)
		idname = lexem->getIdentifier();
}

void Parser::
declare(Poliz *poliz)
{
	int res;
	Variable *v = new Variable(0);
	poliz->insert(v);
	Identifier ident(dtype, v->getPointer());
	res = symbolTable.insert(idname, ident);
	if (res < 0)
		errx(EXIT_FAILURE, "Identifier is already declared!");
}

void Parser::
checkId(Poliz *poliz)
{
	Identifier *identifier = symbolTable.find(idname);
	if (identifier) {
		typeStack.push(identifier->type);
		poliz->insert(new ConstInt((intptr_t) identifier->ptr));
	} else {
		errx(EXIT_FAILURE, "Not declared!");
	}
}

void Parser::
assignment(Poliz *poliz)
{
	if (typeStack.pop() != typeStack.pop())
		errx(EXIT_FAILURE, "Assigning wrong type!");
	poliz->insert(new Inst_mov);
}

void Parser::
checkType()
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
binaryOperation(Poliz *poliz) //TODO redesign
{
	OpType type = lexem->getOpType();
	int curPrecedence = precedence[type];
	while (opStack.getLength() &&
	       precedence[opStack.front()] <= curPrecedence) {
		insertInstruction(poliz, opStack.pop());
		checkType();
	}
	opStack.push(type);
}

void Parser::
gvar(Poliz *poliz)
{
	for (;;) {
		if (lextype == COMMA) {
			declare(poliz);
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
	declare(poliz);
}

void Parser::
var(Poliz *poliz)
{
	//TODO Syntax analysis for local variables
	poliz = poliz;
}

void Parser::
unaryOperation()
{
	puts("UNARY");
	OpType type = lexem->getOpType();
	switch (type) {
	case ADD:
		return;
	case SUB:
		type = NEG;
		break;
	case MUL:
		type = DEREFERENCE;
	case NOT:
	case BITNOT:
		break;
	default:
		errx(EXIT_FAILURE, "Not unary operation!");
	}
	opStack.push(type);
}

void Parser::
insertInstruction(Poliz *poliz, OpType type)
{
	switch (type) {
	case ADD:
		poliz->insert(new Inst_add);
		break;
	case SUB:
		poliz->insert(new Inst_sub);
		break;
	case MUL:
		poliz->insert(new Inst_mul);
		break;
	case DIV:
		poliz->insert(new Inst_div);
		break;
	default:
		errx(EXIT_FAILURE, "NOT IMPLEMENTED");
	}
}

void Parser::
expression(Poliz *poliz)
{
	for (;;) {
		get();
		if (lextype == CONST_INT) {
			typeStack.push(INT);
			poliz->insert(new ConstInt(lexem->getInt()));
		} else if (lextype == IDENTIFIER) {
			checkId(poliz);
			poliz->insert(new Inst_dereference());
		} else if (lextype == LPARENTHESIS) {
			opStack.push(LParentOp);
			expression(poliz);
			if (lextype != RPARENTHESIS)
				errx(EXIT_FAILURE, "')' expected!");
		} else if (lextype == OPERATOR) {
			unaryOperation();
			continue;
		} else {
			errx(EXIT_FAILURE, "BAD EXPRESSION!");
		}
		get();
		if (lextype != OPERATOR)
			break;
		binaryOperation(poliz);
	}
	while (opStack.getLength()) {
		OpType type;
		type = opStack.pop();
		if (type == LParentOp)
			break;
		insertInstruction(poliz, type);
		checkType();
	}
}

void Parser::
keyword(Poliz *poliz)
{
	//TODO goto, if, while, read, write
	poliz = poliz;
}

void Parser::
statement(Poliz *poliz)
{
	get();
	if (lextype == SEMICOLON)
		return;
	if (lextype == IDENTIFIER) {
		get();
		if (lextype != EQUALSIGN)
			errx(EXIT_FAILURE, "'=' expected!");
		checkId(poliz);
		expression(poliz);
		assignment(poliz);
		if (lextype != SEMICOLON)
			errx(EXIT_FAILURE, "';' expected!");
	} else if (lextype == STATEMENT) {
		keyword(poliz);
	} else if (lextype == BEGIN) {
		body(poliz);
	}

}

void Parser::
body(Poliz *poliz)
{
	for (;;) {
		statement(poliz);
		if (lextype == END)
			break;
	}
}

void Parser::
function(Poliz *poliz)
{
	get();
	if (lextype != RPARENTHESIS)
		errx(EXIT_FAILURE, "')' expected!");
	get();
	if (lextype == BEGIN)
		body(poliz);
	else
		errx(EXIT_FAILURE, "'{' expected!");
}

void Parser::
dataType()
{
	if (lextype != DATA_TYPE)
		errx(EXIT_FAILURE, "Data type expected!");
	dtype = lexem->getDataType();
}

Poliz* Parser::
analyze()
{
	Poliz *poliz = new Poliz;
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
			function(poliz);
		else
			gvar(poliz);
	}
	return poliz;
}
