#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <string.h>

#define LEN(X) (sizeof(X) / sizeof((X)[0]))

template <class Type> class Stack {
	Type *item;
	int tail;
public:
	Stack(const int size) { item = new Type[size]; tail = 0; }
	~Stack() { delete [] item; }
	void push(const Type &value) { item[tail++] = value; }
	Type pop() { return item[--tail]; }
	Type front() const { return item[tail - 1]; }
	int getSize() const { return tail; }
};


/* Funny constants */
static const int maxWidth = 81;
static const int maxOpLen = 7;

enum LEXEMCLASS {
	BASE,
	NUMBER,
	OPERATION
};

enum OPTYPE {
	LBRACKET, RBRACKET,
	PLUS, MINUS,
	MULTIPLY,
	NOT_OPERATION
};

static const struct Operation {
	const char verb[maxOpLen];
	const int priority;
} operation[] = {
	[LBRACKET] = { "(", -1 },
	[RBRACKET] = { ")", -1 },
	[PLUS] = { "+", 0 },
	[MINUS] = { "-", 0 },
	[MULTIPLY] = { "*", 1 }
};
static const int operations = LEN(operation);

class Line {
	char str[maxWidth];
	int len, pos;
public:
	Line() : str(""), len(0), pos(0) {}
	char *read();
	void skipSpaces();
	int scanInt(int &dest);
	OPTYPE scanOperation();
	char getTop() const { return str[pos]; }
	bool notOver() const { return pos < len; }
};

class Lexem {
	LEXEMCLASS lexClass;
public:
	Lexem() {}
	Lexem(LEXEMCLASS cl) : lexClass(cl) {}
	LEXEMCLASS getClass() const { return lexClass; }
	virtual void print() = 0;
};

class Number: public Lexem {
	int value;
public:
	Number(int v) : Lexem(NUMBER), value(v) {}
	int getValue() const { return value; }
	void print();
};

class BinOp: public Lexem {
	OPTYPE type;
public:
	BinOp(const OPTYPE &t) : Lexem(OPERATION), type(t) {}
	OPTYPE getType() const { return type; }
	int getPriority() const { return operation[type].priority; }
	int getValue(const Number &l, const Number &r) const;
	bool buildPoliz(Stack<Lexem *> &stack);
	void print();
};

class Poliz {
	Lexem *lexem[maxWidth];
	int len;
public:
	Poliz() : len(0) {}
	~Poliz();
	Lexem *getLexem(int i) const { return lexem[i]; }
	void clear();
	void push(Lexem *l) { lexem[len++] = l; }
	int parse(Line &line);
	int getLen() const { return len; }
	int build();
	int evaluate();
	void print();
};

void Line::
skipSpaces()
{
	while (isspace(str[pos]))
		pos++;
}

char * Line::
read()
{
	char *res;

	pos = 0;
	res = fgets(str, sizeof str, stdin);
	if (!res) {
		len = 0;
	} else {
		len = strlen(str);
		if (str[len - 1] == '\n')
			str[--len] = '\0';
	}
	return res;
}

int Line::
scanInt(int &dest)
{
	int offset;

	if (sscanf(str + pos, "%d%n", &dest, &offset) < 1) {
		//warnx("Integer input failed!");
		return -1;
	}
	pos += offset;
	return 0;
}

OPTYPE Line::
scanOperation()
{
	int tmp, i;

	this->skipSpaces(); /* side-effect */
	for (i = 0; i < operations; i++) {
		tmp = strlen(operation[i].verb);
		if (len - pos < tmp)
			continue;
		if (!strncmp(str + pos, operation[i].verb, tmp)) {
			pos += tmp;
			break;
		}
	}
	return (OPTYPE) i;
}


int Poliz::
parse(Line &line)
{
	char ch;
	OPTYPE optype;
	int integer;

	while (line.notOver()) {
		optype = line.scanOperation();
		if (optype != NOT_OPERATION) {
			push(new BinOp(optype));
			continue;
		}
		if (line.scanInt(integer) >= 0) {
			push(new Number(integer));
			continue;
		}
		errx(1, "Wrong input!");
	}
	return 0;
}

void Poliz::
clear()
{
	for (int i = 0; i < len; i++)
		delete lexem[i];
	len = 0;
}

Poliz::
~Poliz()
{
	this->clear();
}

int Poliz::
build()
{
	int pos = 0;
	Stack<BinOp *> stack(maxWidth);
	BinOp *binop;

	for (int i = 0; i < len; i++) {
		switch (lexem[i]->getClass()) {
		case NUMBER:
			lexem[pos++] = lexem[i];
			break;
		case OPERATION:
			binop = (BinOp *) lexem[i];
			switch (binop->getType()) {
			case LBRACKET:
				stack.push(binop);
				break;
			case RBRACKET:
				while (stack.front()->getType() != LBRACKET)
					lexem[pos++] = stack.pop();
				delete lexem[i];
				delete stack.pop();
				break;
			default:
				while (stack.getSize() &&
				       stack.front()->getPriority() >=
				       binop->getPriority())
					lexem[pos++] = stack.pop();
				stack.push(binop);
			}
			break;
		}
	}
	while (stack.getSize())
		lexem[pos++] = stack.pop();
	len = pos;
	return 0;
}

void Poliz::
print()
{
	for (int i = 0; i < len; i++) {
		lexem[i]->print();
		putchar(' ');
	}
	putchar('\n');
}

void Number::
print()
{
	printf("%d", value);
}

void BinOp::
print()
{
	fputs(operation[type].verb, stdout);
}

int Poliz::
evaluate()
{
	BinOp *binop;
	Stack<int> stack(maxWidth);
	int foo, bar;

	for (int i = 0; i < len; i++) {
		if (lexem[i]->getClass() == NUMBER) {
			stack.push(((Number *) lexem[i])->getValue());
			continue;
		}
		binop = (BinOp *) lexem[i];
		bar = stack.pop();
		foo = stack.pop();
		switch (binop->getType()) {
		case PLUS:
			foo += bar;
			break;
		case MINUS:
			foo -= bar;
			break;
		case MULTIPLY:
			foo *= bar;
			break;
		}
		stack.push(foo);
	}
	return stack.pop();
}

int
main()
{
	Line line;
	Poliz poliz;
	int value;

	while (line.read()) {
		poliz.parse(line);
		poliz.build();
		value = poliz.evaluate();
		//poliz.print();
		printf("%d\n", value);
		poliz.clear();
	}
	return 0;
}
