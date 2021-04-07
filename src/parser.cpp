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
	token = list.next();
	tokenType =  token->getType();
	if (tokenType == IDENTIFIER)
		idname = token->getIdentifier();
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
		errx(EXIT_FAILURE, err_rep_decl, token->getLineNumber());
}

void Parser::
checkId(Poliz *poliz)
{
	Identifier *identifier = symbolTable.find(idname);
	if (identifier) {
		typeStack.push(identifier->type);
		poliz->insert(new ConstInt(reinterpret_cast<intptr_t>(identifier->ptr)));
	} else {
		errx(EXIT_FAILURE, err_not_decl, token->getLineNumber());
	}
}

void Parser::
assignment(Poliz *poliz)
{
	if (typeStack.pop() != typeStack.pop())
		errx(EXIT_FAILURE, err_type_assign, token->getLineNumber());
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
		errx(EXIT_FAILURE, err_type, token->getLineNumber());
}

void Parser::
binaryOperation(Poliz *poliz)
{
	OpType type = token->getOpType();
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
		if (tokenType == COMMA) {
			declare(poliz);
			get();
			if (tokenType != IDENTIFIER)
				errx(EXIT_FAILURE, err_identifier,
				     token->getLineNumber());
			get();
			continue;
		}
		if (tokenType == SEMICOLON)
			break;
		errx(EXIT_FAILURE, err_semicolon,
		     token->getLineNumber());
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
	OpType type = token->getOpType();
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
		errx(EXIT_FAILURE, err_unary, token->getLineNumber());
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
		     token->getLineNumber());
	}
}

void Parser::
expressionArg(Poliz *poliz)
{
	if (tokenType == CONST_INT) {
		typeStack.push(INT);
		poliz->insert(new ConstInt(token->getInt()));
	} else if (tokenType == IDENTIFIER) {
		checkId(poliz);
		poliz->insert(new Inst_dereference());
	} else if (tokenType == LPARENTHESIS) {
		opStack.push(LParentOp);
		expression(poliz);
		if (tokenType != RPARENTHESIS)
			errx(EXIT_FAILURE, err_rparentheses,
			     token->getLineNumber());
	} else {
		errx(EXIT_FAILURE, err_expression,
		     token->getLineNumber());
	}
}

void Parser::
expression(Poliz *poliz) /* TODO redesign */
{
	for (;;) {
		get();
		if (tokenType == OPERATOR) {
			unaryOperation();
			continue;
		}
		expressionArg(poliz);
		get();
		if (tokenType != OPERATOR)
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
	if (tokenType == SEMICOLON)
		return;
	if (tokenType == IDENTIFIER) {
		get();
		if (tokenType != EQUALSIGN)
			errx(EXIT_FAILURE, err_equal,
			     token->getLineNumber());
		checkId(poliz);
		expression(poliz);
		assignment(poliz);
		if (tokenType != SEMICOLON)
			errx(EXIT_FAILURE, err_semicolon,
			     token->getLineNumber());
	} else if (tokenType == STATEMENT) {
		keyword(poliz);
	} else if (tokenType == BEGIN) {
		body(poliz);
	}

}

void Parser::
body(Poliz *poliz)
{
	for (;;) {
		get();
		if (tokenType == END)
			break;
		if (tokenType == TOKEN_NULL)
			err(EXIT_FAILURE, err_rbrace,
			    token->getLineNumber());
		statement(poliz);
	}
}

void Parser::
function(Poliz *poliz)
{
	get();
	if (tokenType != RPARENTHESIS)
		errx(EXIT_FAILURE, err_rparentheses,
		     token->getLineNumber());
	get();
	if (tokenType == BEGIN)
		body(poliz);
	else
		errx(EXIT_FAILURE, err_rbrace,
		     token->getLineNumber());
}

void Parser::
dataType()
{
	if (tokenType != DATA_TYPE)
		errx(EXIT_FAILURE, err_dtype, token->getLineNumber());
	dtype = token->getDataType();
}

Poliz* Parser::
analyze()
{
	Poliz *poliz = new Poliz;
	for (;;) {
		get();
		if (tokenType == TOKEN_NULL)
			break;
		dataType();
		get();
		if (tokenType != IDENTIFIER)
			errx(EXIT_FAILURE, err_identifier,
			     token->getLineNumber());
		get();
		if (tokenType == LPARENTHESIS)
			function(poliz);
		else
			gvar(poliz);
	}
	return poliz;
}
