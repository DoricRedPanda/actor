#include <err.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

static const char delimiters[] = " \f\n\r\t\v,;()[]{}*:";

static const char *dataType[] = {
	"int",
	NULL
};

static const char *opname[] = {
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
	"!", "~",
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

static const char err_wordlen[] = "\t%d\t|\tWord is too long";
static const char err_int_suffix[] = "\t%d\t|\tInvalid suffix on integer constant";
static const char err_invalid_char[] = "\t%d\t|\tUnexpected character %c";
static const char err_op_suffix[] = "\t%d\t|\tInvalid suffix on operation";
static const char err_invalid_op[] = "\t%d\t|\tUnrecognized operator";
static const char err_unreachable[] = "\t%d\t|\tLexical analyzer state should be unreachable!";


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
		errx(EXIT_FAILURE, err_wordlen, line);
	buf[pos++] = ch;
}

void Lexer::
number(LexemList *list)
{
	int res;
	for (;;) {
		push();
		get();
		if (isdigit(ch))
			continue;
		if (strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, err_int_suffix, line);
	}
	unget();
	end();
	sscanf(buf, "%d", &res);
	list->insert(Lexem(line, res));
}

/* sign = sign of number | operation */
void Lexer::
sign(LexemList *list)
{
	push();
	get();
	if (isdigit(ch)) {
		number(list);
	} else if (ispunct(ch)) {
		operation(list);
	} else {
		OpType type = buf[0] == '+' ? ADD : SUB;
		list->insert(Lexem(line, type));
		unget();
	}
}

/* word = statement | identifier | data type */
void Lexer::
word(LexemList *list)
{
	int type;
	for (;;) {
		push();
		get();
		if (isalnum(ch))
			continue;
		if (strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, err_invalid_char, line, ch);
	}
	unget();
	end();
	type = look(statement);
	if (type >= 0) {
		list->insert(Lexem(line, static_cast<StatementType>(type)));
		return;
	}
	type = look(dataType);
	if (type >= 0) {
		list->insert(Lexem(line, static_cast<DataType>(type)));
		return;
	}
	list->insert(Lexem(line, buf));
}

void Lexer::
operation(LexemList *list)
{
	int type;
	for (;;) {
		push();
		get();
		if (ispunct(ch))
			continue;
		if (strchr(delimiters, ch))
			break;
		errx(EXIT_FAILURE, err_op_suffix, line);
	}
	unget();
	end();
	type = look(opname);
	if (type < 0) {
		if (!strcmp(buf, "=")) {
			list->insert(Lexem(line, EQUALSIGN));
			return;
		}
		errx(EXIT_FAILURE, err_invalid_op, line);
	}
	list->insert(Lexem(line, static_cast<OpType>(type)));
}

void Lexer::
delimiter(LexemList *list)
{
	switch(ch) {
	case ',':
		list->insert(Lexem(line, COMMA));
		break;
	case ':':
		list->insert(Lexem(line, TWO_SPOT));
		break;
	case '*':
		list->insert(Lexem(line, MUL));
		break;
	case '{':
		list->insert(Lexem(line, BEGIN));
		break;
	case '}':
		list->insert(Lexem(line, END));
		break;
	case '[':
		list->insert(Lexem(line, LBRACKET));
		break;
	case ']':
		list->insert(Lexem(line, RBRACKET));
		break;
	case '(':
		list->insert(Lexem(line, LPARENTHESIS));
		break;
	case ')':
		list->insert(Lexem(line, RPARENTHESIS));
		break;
	case ';':
		list->insert(Lexem(line, SEMICOLON));
		break;
	case '\n':
		line++;
		break;
	}
}


/* Check DFA to understand */
LexemList* Lexer::
analyze()
{
	LexemList *list = new LexemList;
	for (pos = 0;; pos = 0) {
		get();
		if (ch == EOF)
			break;
		if (strchr(delimiters, ch))
			delimiter(list);
		else if (isdigit(ch))
			number(list);
		else if (ch == '+' || ch == '-')
			sign(list);
		else if (isalpha(ch))
			word(list);
		else if (ispunct(ch))
			operation(list);
		else
			errx(EXIT_FAILURE, err_invalid_char, line, ch);
		clear();
	}
	list->insert(Lexem(line, LEX_NULL));
	return list;
}

