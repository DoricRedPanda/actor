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

static const char err_main[] = "\t\t|\tmain function not found";
static const char err_bad_decl[] = "\t%d\t|\tBad declaration";
static const char err_not_decl[] = "\t%d\t|\tNot declared";
static const char err_type_assign[] = "\t%d\t|\tAssignment requires matching types";
static const char err_identifier[] = "\t%d\t|\tIdentifier expected";
static const char err_semicolon[] = "\t%d\t|\tSemicolon expected";
static const char err_type_mismatch[] = "\t%d\t|\tMismatching types";
static const char err_unary[] = "\t%d\t|\tUnary operation expected";
static const char err_lparent[] = "\t%d\t|\t'(' expected";
static const char err_rparent[] = "\t%d\t|\t')' expected";
static const char err_lbracket[] = "\t%d\t|\t'[' expected";
static const char err_rbracket[] = "\t%d\t|\t']' expected";
static const char err_expression[] = "\t%d\t|\tInvalid expression";
static const char err_equal[] = "\t%d\t|\t'=' expected";
static const char err_lbrace[] = "\t%d\t|\t'{' expected";
static const char err_not_closed[] = "\t%d\t|\tBlock not closed";
static const char err_dtype[] = "\t%d\t|\tData type expected";
static const char err_implementation[] = "\t%d\t|\tNot implemented";
static const char err_undefined_label[] = "\t%d\t|\tUndefined label";
static const char err_comma[] = "\t%d\t|\t',' expected";
static const char err_ident_not_label[] = "\t%d\t|\tgoto expects label";
static const char err_bad_statement[] = "\t%d\t|\tStatement expected";
static const char err_integer[] = "\t%d\t|\tInteger number expected";
static const char err_identifier_statement[] = "\t%d\t|\tBad type declared";

size_t DataType::
getSize()
{
	switch (bt) {
	case ARRAY:
		return size * ptr->getSize();
	default:
		return size;
	}
}

bool LocalIdentifier::
equals(BaseType bt) const
{
	return type->bt == bt;
}

bool Identifier::
equals(BaseType bt) const
{
	return type->bt == bt;
}

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
declare(Poliz *poliz, DataType *type)
{
	int res = -1;
	void *polizItemPtr;
	if (type->bt == ARRAY) {
		Array *arr = new Array(type->getSize());
		poliz->insert(arr);
		polizItemPtr = arr->getPointer();
	} else if (type->bt == INT) {
		Variable *v = new Variable(0);
		poliz->insert(v);
		polizItemPtr = v->getPointer();
	} else if (type->bt == PROCEDURE || type->bt == FUNCTION) {
		poliz->insert(new Label(NULL));
		polizItemPtr = poliz->getTail();
	} else {
		errx(EXIT_FAILURE, err_bad_decl, token->getPos());
	}
	Identifier *ident = new Identifier(type, polizItemPtr);
	res = symbolTable.insert(idname, ident);
	if (res < 0)
		errx(EXIT_FAILURE, err_bad_decl, token->getPos());
}

/* FIXME redesign */
void Parser::
declareLocalVariable(DataType *type,
                     LocalSymbolTable &localST, size_t index)
{
	int res = -1;
	if (type->bt != INT)
		errx(EXIT_FAILURE, err_implementation, token->getPos());
	LocalIdentifier *ident = new LocalIdentifier(type, index);
	res = localST.insert(idname, ident);
	if (res < 0)
		errx(EXIT_FAILURE, err_bad_decl, token->getPos());
}

/* FIXME redesign */
void Parser::
checkLocalIdentifier(Poliz *poliz, LocalIdentifier *identifier)
{
	if (identifier->equals(INT)) {
		typeStack.push(INT);
		poliz->insert(new ConstInt(identifier->ptr));
	} else {
		errx(EXIT_FAILURE, err_implementation, token->getPos());
	}
}

/* FIXME redesign */
void Parser::
checkIdentifier(Poliz *poliz, Identifier *identifier,
                LocalSymbolTable &localST)
{
	if (!identifier)
		errx(EXIT_FAILURE, err_not_decl, token->getPos());
	if (identifier->equals(ARRAY)) {
		poliz->insert(new IntAddress(identifier->ptr));
		expect(LBRACKET, err_lbracket);
		expression(poliz, localST);
		expect(RBRACKET, err_rbracket);
		poliz->insert(new Inst_AddrPlus());
		typeStack.push(INT);
	} else if (identifier->equals(INT)) {
		typeStack.push(INT);
		poliz->insert(new IntAddress(identifier->ptr));
	} else {
		errx(EXIT_FAILURE, err_implementation, token->getPos());
	}
}

void Parser::
subprogramCall(Poliz *poliz, Identifier *identifier)
{
	PolizItemNode *addr;
	expect(LPARENTHESIS, err_lparent);
	expect(RPARENTHESIS, err_rparent);
	addr = static_cast<PolizItemNode*>(identifier->ptr);
	poliz->insert(new Label(addr));
	poliz->insert(new SubprogramCall());
}

/* FIXME redesign */
void Parser::
identifierStatement(Poliz *poliz, LocalSymbolTable &localST)
{
	LocalIdentifier *localIdentifier = localST.find(idname);
	if (localIdentifier) {
		checkLocalIdentifier(poliz, localIdentifier);
		assignment(poliz, localST);
		poliz->insert(new Inst_movStack);
		return;
	}
	Identifier *identifier = symbolTable.find(idname);
	if (!identifier)
		errx(EXIT_FAILURE, err_not_decl, token->getPos());
	if (identifier->equals(PROCEDURE)) {
		subprogramCall(poliz, identifier);
		expect(SEMICOLON, err_semicolon);
	} else {
		checkIdentifier(poliz, identifier, localST);
		assignment(poliz, localST);
		poliz->insert(new Inst_mov);
	}
}

void Parser::
checkType()
{
	BaseType foo, bar;
	bar = typeStack.pop();
	foo = typeStack.pop();
	if (foo == bar)
		typeStack.push(foo);
	else
		errx(EXIT_FAILURE, err_type_mismatch, token->getPos());
}

void Parser::
checkType(BaseType expectedType)
{
	BaseType type = typeStack.pop();
	if (type != expectedType)
		errx(EXIT_FAILURE, err_type_mismatch, token->getPos());
}

void Parser::
binaryOperation(Poliz *poliz, Stack<OpType> &opStack)
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
	DataType *type;
	for (;;) {
		type = new DataType(dtype);
		if (tokenType == LBRACKET) {
			expect(CONST_INT, err_integer);
			type = new DataType(type, token->getInt());
			expect(RBRACKET, err_rbracket);
			get();
		}
		declare(poliz, type);
		if (tokenType == SEMICOLON)
			break;
		if (tokenType != COMMA)
			errx(EXIT_FAILURE, err_comma, token->getPos());
		expect(IDENTIFIER, err_identifier);
		get();
	}
}

long Parser::
var(LocalSymbolTable &localST)
{
	long count = 0;
	for (;;) {
		get();
		if (tokenType != DATA_TYPE) {
			isSkippingNextGet = true;
			break;
		}
		dtype = token->getBaseType();
		for (;;) {
			DataType *type = new DataType(dtype);
			expect(IDENTIFIER, err_identifier);
			count++;
			declareLocalVariable(type, localST, count);
			get();
			if (tokenType == SEMICOLON)
				break;
			if (tokenType != COMMA)
				errx(EXIT_FAILURE, err_comma, token->getPos());
		}
	}
	return count;
}

void Parser::
unaryOperation(Stack<OpType> &opStack)
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

/* FIXME redesign */
void Parser::
expressionArg(Poliz *poliz, LocalSymbolTable &localST)
{
	if (tokenType == CONST_INT) {
		typeStack.push(INT);
		poliz->insert(new ConstInt(token->getInt()));
	} else if (tokenType == IDENTIFIER) {
		LocalIdentifier *localIdentifier = localST.find(idname);
		if (localIdentifier) {
			checkLocalIdentifier(poliz, localIdentifier);
			poliz->insert(new Inst_dereferenceStack());
		} else {
			Identifier *identifier = symbolTable.find(idname);
			if (identifier && identifier->equals(FUNCTION)) {
				subprogramCall(poliz, identifier);
			} else {
				checkIdentifier(poliz, identifier, localST);
				poliz->insert(new Inst_dereference());
			}
		}
	} else if (tokenType == LPARENTHESIS) {
		expression(poliz, localST);
		expect(RPARENTHESIS, err_rparent);
	} else {
		errx(EXIT_FAILURE, err_expression, token->getPos());
	}
}

void Parser::
expression(Poliz *poliz, LocalSymbolTable &localST)
{
	Stack<OpType> opStack(STACK_SIZE);
	for (;;) {
		get();
		if (tokenType == OPERATOR) {
			unaryOperation(opStack);
			continue;
		}
		expressionArg(poliz, localST);
		get();
		if (tokenType != OPERATOR)
			break;
		binaryOperation(poliz, opStack);
	}
	flushOperationStack(poliz, opStack);
	isSkippingNextGet = true;
}

void Parser::
flushOperationStack(Poliz *poliz, Stack<OpType> &opStack)
{
	while (opStack.getLength()) {
		OpType type;
		type = opStack.pop();
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
		if (!ident->equals(LABEL))
			errx(EXIT_FAILURE, err_ident_not_label, pos);
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
branching(Poliz *poliz, LocalSymbolTable &localST)
{
	expect(LPARENTHESIS, err_lparent);
	expression(poliz, localST);
	checkType(INT);
	expect(RPARENTHESIS, err_rparent);
	Label *labelFalse = new Label(NULL);
	poliz->insert(labelFalse);
	poliz->insert(new PolizOpGoFalse);
	statement(poliz, localST);
	get();
	if (tokenType == STATEMENT && token->getStatementType() == ELSE) {
		Label *labelTrue = new Label(NULL);
		poliz->insert(labelTrue);
		poliz->insert(new PolizOpGo);
		PolizItemNode *addr = poliz->getTail();
		labelFalse->set(addr);
		statement(poliz, localST);
		addr = poliz->getTail();
		labelTrue->set(addr);
	} else {
		PolizItemNode *addr = poliz->getTail();
		labelFalse->set(addr);
		isSkippingNextGet = true;
	}
}

void Parser::
cycle(Poliz *poliz, LocalSymbolTable &localST)
{
	PolizItemNode *start = poliz->getTail();
	expect(LPARENTHESIS, err_lparent);
	expression(poliz, localST);
	checkType(INT);
	expect(RPARENTHESIS, err_rparent);
	Label *labelFinish = new Label(NULL);
	poliz->insert(labelFinish);
	poliz->insert(new PolizOpGoFalse);
	statement(poliz, localST);
	poliz->insert(new Label(start));
	poliz->insert(new PolizOpGo);
	PolizItemNode *finish = poliz->getTail();
	labelFinish->set(finish);
}

void Parser::
writing(Poliz *poliz, LocalSymbolTable &localST)
{
	expression(poliz, localST);
	checkType(INT);
	expect(SEMICOLON, err_semicolon);
	poliz->insert(new Inst_print);
}

void Parser::
keyword(Poliz *poliz, LocalSymbolTable &localST)
{
	StatementType type = token->getStatementType();
	switch (type) {
	case GOTO:
		statementGoto(poliz);
		break;
	case IF:
		branching(poliz, localST);
		break;
	case WHILE:
		cycle(poliz, localST);
		break;
	case WRITE:
		writing(poliz, localST);
		break;
	case RETURN:
		returnStatement(poliz, localST);
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
	Identifier *ident = new Identifier(new DataType(LABEL), addr);
	res = symbolTable.insert(token->getIdentifier(), ident);
	if (res < 0)
		errx(EXIT_FAILURE, err_bad_decl, token->getPos());
}

void Parser::
assignment(Poliz *poliz, LocalSymbolTable &localST)
{
	expect(EQUALSIGN, err_equal);
	expression(poliz, localST);
	expect(SEMICOLON, err_semicolon);
	checkType();
}

void Parser::
returnStatement(Poliz *poliz, LocalSymbolTable &localST)
{
	expression(poliz, localST);
	expect(SEMICOLON, err_semicolon);
	poliz->insert(new FunctionReturn());
}

void Parser::
statement(Poliz *poliz, LocalSymbolTable &localST)
{
	get();
	switch (tokenType) {
	case SEMICOLON:
		break;
	case LABEL_DEF:
		declareLabel(poliz);
		statement(poliz, localST);
		break;
	case IDENTIFIER:
		identifierStatement(poliz, localST);
		break;
	case STATEMENT:
		keyword(poliz, localST);
		break;
	case BEGIN:
		body(poliz, localST);
		break;
	case RETURN:
		returnStatement(poliz, localST);
		break;
	default:
		errx(EXIT_FAILURE, err_bad_statement, token->getPos());
	}
}

void Parser::
body(Poliz *poliz, LocalSymbolTable &localST)
{
	for (;;) {
		get();
		if (tokenType == END)
			break;
		if (tokenType == TOKEN_NULL)
			errx(EXIT_FAILURE, err_not_closed, token->getPos());
		isSkippingNextGet = true;
		statement(poliz, localST);
	}
}

/* FIXME redesign */
void Parser::
subprogram(Poliz *poliz)
{
	LocalSymbolTable localSymbolTable;
	DataType *type;
	if (dtype == VOID) {
		type = new DataType(PROCEDURE);
	} else if (dtype == INT) {
		type = new DataType(FUNCTION);
	}
	expect(RPARENTHESIS, err_rparent);
	declare(poliz, type);
	expect(BEGIN, err_lbrace);
	size_t localVariablesSize = var(localSymbolTable);
	poliz->insert(new Subprogram(localVariablesSize));
	body(poliz, localSymbolTable);
	poliz->insert(new ProcedureReturn());
}

void Parser::
dataType()
{
	if (tokenType != DATA_TYPE)
		errx(EXIT_FAILURE, err_dtype, token->getPos());
	dtype = token->getBaseType();
}

void Parser::
insertLabels()
{
	while (labelStack.getLength()) {
		UndefinedLabel item = labelStack.pop();
		Identifier *ident = symbolTable.find(item.name);
		if (!ident)
			errx(EXIT_FAILURE, err_undefined_label, item.line);
		if (!ident->equals(LABEL))
			errx(EXIT_FAILURE, err_ident_not_label, item.line);
		PolizItemNode *addr;
		addr = static_cast<PolizItemNode*>(ident->ptr);
		item.label->set(addr);
	}
}

Poliz* Parser::
analyze()
{
	Poliz *poliz = new Poliz;
	Label *label = new Label(NULL);
	poliz->insert(label);
	poliz->insert(new SubprogramCall());
	poliz->insert(new PolizExit());
	for (;;) {
		get();
		if (tokenType == TOKEN_NULL)
			break;
		dataType();
		expect(IDENTIFIER, err_identifier);
		get();
		if (tokenType == LPARENTHESIS)
			subprogram(poliz);
		else
			gvar(poliz);
	}
	insertLabels();
	Identifier *identifier = symbolTable.find("main");
	if (!identifier)
		errx(EXIT_FAILURE, err_main);
	PolizItemNode *mainAddress;
	mainAddress = static_cast<PolizItemNode*>(identifier->ptr);
	label->set(mainAddress);
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
