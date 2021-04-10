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
static const char err_lparent[] = "\t%d\t|\t'(' expected";
static const char err_rparent[] = "\t%d\t|\t')' expected";
static const char err_expression[] = "\t%d\t|\tInvalid expression";
static const char err_equal[] = "\t%d\t|\t'=' expected";
static const char err_lbrace[] = "\t%d\t|\t'{' expected";
static const char err_not_closed[] = "\t%d\t|\tBlock not closed";
static const char err_dtype[] = "\t%d\t|\tData type expected";
static const char err_implementation[] = "\t%d\t|\tNot implemented";
static const char err_label[] = "\t%d\t|\tUndefined label";
static const char err_bad_statement[] = "\t%d\t|\tStatement expected";

void Parser::
get()
{
	if (isSkippingNextGet) {
		isSkippingNextGet = false;
		return;
	}
	token = list.next();
	tokenType =  token->getType();
	if (tokenType == IDENTIFIER)
		idname = token->getIdentifier();
}

void Parser::
expect(TokenType type, const char errmsg[])
{
	get();
	if (tokenType != type)
		errx(EXIT_FAILURE, errmsg, token->getPos());
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
		errx(EXIT_FAILURE, err_rep_decl, token->getPos());
}

void Parser::
checkId(Poliz *poliz)
{
	Identifier *identifier = symbolTable.find(idname);
	if (identifier) {
		typeStack.push(identifier->type);
		poliz->insert(new IntAddress(identifier->ptr));
	} else {
		errx(EXIT_FAILURE, err_not_decl, token->getPos());
	}
}

void Parser::
checkAssignment(Poliz *poliz)
{
	if (typeStack.pop() != typeStack.pop())
		errx(EXIT_FAILURE, err_type_assign, token->getPos());
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
		errx(EXIT_FAILURE, err_type, token->getPos());
}

void Parser::
binaryOperation(Poliz *poliz)
{
	OpType type = token->getOpType();
	int curPrecedence = precedence[type];
	while (opStack.getLength() &&
	       precedence[opStack.front()] <= curPrecedence) {
		insertInstruction(poliz, opStack.pop());
	}
	opStack.push(type);
}

void Parser::
gvar(Poliz *poliz)
{
	while (tokenType == COMMA) {
		declare(poliz);
		expect(IDENTIFIER, err_identifier);
		get();
	}
	if (tokenType != SEMICOLON)
		errx(EXIT_FAILURE, err_semicolon, token->getPos());
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
		errx(EXIT_FAILURE, err_unary, token->getPos());
	}
	opStack.push(type);
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
			errx(EXIT_FAILURE, err_rparent, token->getPos());
	} else {
		errx(EXIT_FAILURE, err_expression, token->getPos());
	}
}

void Parser::
expression(Poliz *poliz)
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
	flushOperationStack(poliz);
	isSkippingNextGet = true;
}

void Parser::
flushOperationStack(Poliz *poliz)
{
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
statementGoto(Poliz *poliz)
{
	PolizItemNode *addr;
	expect(IDENTIFIER, err_identifier);
	int pos = token->getPos();
	Identifier *ident = symbolTable.find(idname);
	if (ident) {
		addr = static_cast<PolizItemNode*>(ident->ptr);
		poliz->insert(new Label(addr));
	} else {
		Label *label = new Label(NULL);
		poliz->insert(label);
		labelStack.push(UndefinedLabel(idname, label, pos));
	}
	poliz->insert(new PolizOpGo);
	expect(SEMICOLON, err_semicolon);
}

void Parser::
branching(Poliz *poliz)
{
	expect(LPARENTHESIS, err_lparent);
	expression(poliz);
	expect(RPARENTHESIS, err_rparent);
	Label *labelFalse = new Label(NULL);
	poliz->insert(labelFalse);
	poliz->insert(new PolizOpGoFalse);
	statement(poliz);
	get();
	if (tokenType == STATEMENT && token->getStatementType() == ELSE) {
		Label *labelTrue = new Label(NULL);
		poliz->insert(labelTrue);
		poliz->insert(new PolizOpGo);
		PolizItemNode *addr = poliz->getTail();
		labelFalse->set(addr);
		statement(poliz);
		addr = poliz->getTail();
		labelTrue->set(addr);
	} else {
		PolizItemNode *addr = poliz->getTail();
		labelFalse->set(addr);
		isSkippingNextGet = true;
	}
}

void Parser::
cycle(Poliz *poliz)
{
	PolizItemNode *start = poliz->getTail();
	expect(LPARENTHESIS, err_lparent);
	expression(poliz);
	expect(RPARENTHESIS, err_rparent);
	Label *labelFinish = new Label(NULL);
	poliz->insert(labelFinish);
	poliz->insert(new PolizOpGoFalse);
	statement(poliz);
	poliz->insert(new Label(start));
	poliz->insert(new PolizOpGo);
	PolizItemNode *finish = poliz->getTail();
	labelFinish->set(finish);
}

void Parser::
writing(Poliz *poliz)
{
	expression(poliz);
	expect(SEMICOLON, err_semicolon);
	poliz->insert(new Inst_print);
}

void Parser::
keyword(Poliz *poliz)
{
	StatementType type = token->getStatementType();
	switch (type) {
	case GOTO:
		statementGoto(poliz);
		break;
	case IF:
		branching(poliz);
		break;
	case WHILE:
		cycle(poliz);
		break;
	case WRITE:
		writing(poliz);
		break;
	default:
		errx(EXIT_FAILURE, err_implementation, token->getPos());
	}
}

void Parser::
declareLabel(Poliz *poliz)
{
	int res;
	PolizItemNode *addr = poliz->getTail();
	Identifier ident(LABEL, addr);
	res = symbolTable.insert(idname, ident);
	if (res < 0)
		errx(EXIT_FAILURE, err_rep_decl, token->getPos());
}

void Parser::
statement(Poliz *poliz)
{
	get();
	if (tokenType == SEMICOLON)
		return;
	if (tokenType == IDENTIFIER) {
		get();
		if (tokenType == TWO_SPOT) {
			declareLabel(poliz);
			statement(poliz);
			return;
		}
		if (tokenType != EQUALSIGN)
			errx(EXIT_FAILURE, err_equal, token->getPos());
		checkId(poliz);
		expression(poliz);
		expect(SEMICOLON, err_semicolon);
		checkAssignment(poliz);
	} else if (tokenType == STATEMENT) {
		keyword(poliz);
	} else if (tokenType == BEGIN) {
		body(poliz);
	} else {
		errx(EXIT_FAILURE, err_bad_statement, token->getPos());
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
			errx(EXIT_FAILURE, err_not_closed, token->getPos());
		isSkippingNextGet = true;
		statement(poliz);
	}
}

void Parser::
function(Poliz *poliz)
{
	expect(RPARENTHESIS, err_rparent);
	expect(BEGIN, err_lbrace);
	body(poliz);
}

void Parser::
dataType()
{
	if (tokenType != DATA_TYPE)
		errx(EXIT_FAILURE, err_dtype, token->getPos());
	dtype = token->getDataType();
}

void Parser::
insertLabels()
{
	while (labelStack.getLength()) {
		UndefinedLabel item = labelStack.pop();
		Identifier *ident = symbolTable.find(item.id);
		if (!ident)
			errx(EXIT_FAILURE, err_label, item.line);
		PolizItemNode *addr;
		addr = static_cast<PolizItemNode*>(ident->ptr);
		item.label->set(addr);
	}
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
		expect(IDENTIFIER, err_identifier);
		get();
		if (tokenType == LPARENTHESIS)
			function(poliz);
		else
			gvar(poliz);
	}
	insertLabels();
	return poliz;
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
		errx(EXIT_FAILURE, err_implementation, token->getPos());
	}
}
