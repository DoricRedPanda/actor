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

static const char err_rep_decl[] = "\t%d\t|\tIdentifier is already declared";
static const char err_not_decl[] = "\t%d\t|\tNot declared";
static const char err_type_assign[] = "\t%d\t|\tAssignment requires matching types";
static const char err_identifier[] = "\t%d\t|\tIdentifier expected";
static const char err_semicolon[] = "\t%d\t|\tSemicolon expected";
static const char err_type[] = "\t%d\t|\tOperator requires matching types";
static const char err_unary[] = "\t%d\t|\tUnary operation expected";
static const char err_rparentheses[] = "\t%d\t|\t')' expected";
static const char err_expression[] = "\t%d\t|\tInvalid expression";
static const char err_equal[] = "\t%d\t|\t'=' expected";
static const char err_rbrace[] = "\t%d\t|\t'{' expected";
static const char err_dtype[] = "\t%d\t|\tData type expected";
static const char err_implementation[] = "\t%d\t|\tOperator is not implemented";

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
		errx(EXIT_FAILURE, err_rep_decl, lexem->getLineNumber());
}

void Parser::
checkId(Poliz *poliz)
{
	Identifier *identifier = symbolTable.find(idname);
	if (identifier) {
		typeStack.push(identifier->type);
		poliz->insert(new ConstInt(reinterpret_cast<intptr_t>(identifier->ptr)));
	} else {
		errx(EXIT_FAILURE, err_not_decl, lexem->getLineNumber());
	}
}

void Parser::
assignment(Poliz *poliz)
{
	if (typeStack.pop() != typeStack.pop())
		errx(EXIT_FAILURE, err_type_assign, lexem->getLineNumber());
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
		errx(EXIT_FAILURE, err_type, lexem->getLineNumber());
}

void Parser::
binaryOperation(Poliz *poliz)
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
				errx(EXIT_FAILURE, err_identifier,
				     lexem->getLineNumber());
			get();
			continue;
		}
		if (lextype == SEMICOLON)
			break;
		errx(EXIT_FAILURE, err_semicolon,
		     lexem->getLineNumber());
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
	OpType type = lexem->getOpType();
	switch (type) {
	case ADD:
		return; /* unary plus does nothing */
	case SUB:
		type = NEG;
		break;
	case MUL:
		type = DEREFERENCE;
	case NOT:
	case BITNOT:
		break;
	default:
		errx(EXIT_FAILURE, err_unary, lexem->getLineNumber());
	}
	opStack.push(type);
}


/*TODO get rid of ugly switch */
void Parser::
insertInstruction(Poliz *poliz, OpType type)
{
	switch (type) {
	case OR:
		poliz->insert(new Inst_or);
		break;
	case AND:
		poliz->insert(new Inst_and);
		break;
	case BITOR:
		poliz->insert(new Inst_bitor);
		break;
	case XOR:
		poliz->insert(new Inst_xor);
		break;
	case BITAND:
		poliz->insert(new Inst_bitand);
		break;
	case EQ:
		poliz->insert(new Inst_eq);
		break;
	case NEQ:
		poliz->insert(new Inst_neq);
		break;
	case LEQ:
		poliz->insert(new Inst_leq);
		break;
	case LT:
		poliz->insert(new Inst_lt);
		break;
	case GEQ:
		poliz->insert(new Inst_geq);
		break;
	case GT:
		poliz->insert(new Inst_gt);
		break;
	case SHL:
		poliz->insert(new Inst_shl);
		break;
	case SHR:
		poliz->insert(new Inst_shr);
		break;
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
	case MOD:
		poliz->insert(new Inst_mod);
		break;
	case NOT:
		poliz->insert(new Inst_not);
		break;
	case BITNOT:
		poliz->insert(new Inst_bitnot);
		break;
	case NEG:
		poliz->insert(new Inst_neg);
		break;
	default:
		errx(EXIT_FAILURE, err_implementation,
		     lexem->getLineNumber());
	}
}

void Parser::
expression(Poliz *poliz) /* TODO redesign */
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
				errx(EXIT_FAILURE, err_rparentheses,
				     lexem->getLineNumber());
		} else if (lextype == OPERATOR) {
			unaryOperation();
			continue;
		} else {
			errx(EXIT_FAILURE, err_expression,
			     lexem->getLineNumber());
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
			errx(EXIT_FAILURE, err_equal,
			     lexem->getLineNumber());
		checkId(poliz);
		expression(poliz);
		assignment(poliz);
		if (lextype != SEMICOLON)
			errx(EXIT_FAILURE, err_semicolon,
			     lexem->getLineNumber());
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
		errx(EXIT_FAILURE, err_rparentheses,
		     lexem->getLineNumber());
	get();
	if (lextype == BEGIN)
		body(poliz);
	else
		errx(EXIT_FAILURE, err_rbrace,
		     lexem->getLineNumber());
}

void Parser::
dataType()
{
	if (lextype != DATA_TYPE)
		errx(EXIT_FAILURE, err_dtype, lexem->getLineNumber());
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
			errx(EXIT_FAILURE, err_identifier,
			     lexem->getLineNumber());
		get();
		if (lextype == LPARENTHESIS)
			function(poliz);
		else
			gvar(poliz);
	}
	return poliz;
}
