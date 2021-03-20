#include <err.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

static const char delimiters[] = " \f\n\r\t\v,;()[]{}";

static const char *dataType[] {
	"int",
	NULL
};

static const char *opname[] {
	"", "", /* empty names to match enum */
	"||",
	"&&",
	"|",
	"^",
	"&",
	"==", "!=",
	"<=", "<", ">=", ">",
	"<<", ">>",
	"+", "-",
	"*", "/", "%",
	NULL
};

static const char *statement[] = {
	"goto",
	"if",
	"else",
	"while",
	"read",
	"write",
	NULL
};

int Lexer::
look(const char *table[]) const
{
	int i;
	for (i = 0; table[i]; i++)
		if (!strcmp(buf, table[i]))
			return i;
	return -1;
}

void Lexer::
push()
{
	if (pos == MAX_LEXEM_LENGTH)
		errx(EXIT_FAILURE, "BUFFER OVERFLOW");
	buf[pos++] = ch;
}

void Lexer::
number(List<Lexem> &list)
{
	int res;
	for (;;) {
		push();
		get();
		if (isdigit(ch))
			continue;
		if (strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, "BAD LEXEM");
	}
	unget();
	end();
	sscanf(buf, "%d", &res);
	list.insert(Lexem(res));
}

void Lexer::
sign(List<Lexem> &list)
{
	OpType type = ch == '+' ? UPLUS : UMINUS;
	push();
	get();
	if (isdigit(ch)) {
		number(list);
		return;
	}
	if (isalpha(ch)) {
		list.insert(Lexem(type));
		unget();
		return;
	}
	if (strchr(delimiters, ch)) {
		type = buf[0] == '+' ? ADD : SUB;
		list.insert(Lexem(type));
		unget();
		return;
	}
	if (ispunct(ch)) {
		operation(list);
		return;
	}
	errx(EXIT_FAILURE, "BAD LEXEM");
}

void Lexer::
ident(List<Lexem> &list)
{
	for (;;) {
		push();
		get();
		if (isalnum(ch))
			continue;
		if (strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, "BAD LEXEM");
	}
	unget();
	end();
	list.insert(Lexem(buf));
}

void Lexer::
word(List<Lexem> &list)
{
	int type;
	for (;;) {
		push();
		get();
		if (isalpha(ch))
			continue;
		if (isdigit(ch)) {
			ident(list);
			return;
		}
		if (strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, "BAD LEXEM");
	}
	unget();
	end();
	type = look(statement);
	if (type < 0) {
		type = look(dataType);
		if (type < 0)
			list.insert(Lexem(buf));
		else
			list.insert((DataType) type);
		return;
	}
	list.insert(Lexem((StatementType) type));
}

void Lexer::
equalsign(List<Lexem> &list)
{
	list.insert(Lexem(EQUALSIGN));
	get();
	if (!isspace(ch))
		errx(EXIT_FAILURE, "BAD LEXEM");
	unget();
}

void Lexer::
operation(List<Lexem> &list)
{
	int type;
	for (;;) {
		push();
		get();
		if (ispunct(ch))
			continue;
		if (!strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, "BAD LEXEM");
	}
	unget();
	end();
	type = look(opname);
	if (type < 0)
		errx(EXIT_FAILURE, "BAD LEXEM");
	list.insert(Lexem((OpType) type));
}

void Lexer::
delimiter(List<Lexem> &list)
{
	switch(ch) {
	case ',':
		list.insert(Lexem(COMMA));
		break;
	case '{':
		list.insert(Lexem(BEGIN));
		break;
	case '}':
		list.insert(Lexem(END));
		break;
	case '[':
		list.insert(Lexem(LBRACKET));
		break;
	case ']':
		list.insert(Lexem(RBRACKET));
		break;
	case '(':
		list.insert(Lexem(LPARENTHESIS));
		break;
	case ')':
		list.insert(Lexem(RPARENTHESIS));
		break;
	case ';':
		list.insert(Lexem(SEMICOLON));
		break;
	}
}


/* Check DFA to understand */
List<Lexem> Lexer::
run()
{
	List<Lexem> list;
	for (pos = 0;; pos = 0) {
		get();
		if (ch == EOF)
			break;
		if (isspace(ch))
			continue;
		if (strchr(delimiters, ch))
			delimiter(list);
		else if (isdigit(ch))
			number(list);
		else if (ch == '+' || ch == '-')
			sign(list);
		else if (isalpha(ch))
			word(list);
		else if (ch == '=')
			equalsign(list);
		else if (ispunct(ch))
			operation(list);
		else
			errx(EXIT_FAILURE, "BAD LEXEM");
		clear();
	}
	list.insert(LEX_NULL);
	return list;
}

