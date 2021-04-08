#include <err.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

static const char delimiters[] = " \f\n\r\t\v{}[](),:;!*~-";

static const char *lexSpecialSymbol[] = {
	"{", "}",
	"[", "]",
	"(", ")",
	",",
	"=",
	":",
	";",
	NULL
};

static const char *lexDataType[] = {
	"int",
	NULL
};

static const char *lexOperation[] = {
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

static const char *lexStatement[] = {
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
	if (pos == MAX_LEXEME_LENGTH)
		errx(EXIT_FAILURE, err_wordlen, line);
	buf[pos++] = ch;
}

void Lexer::
number(TokenList *list)
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
	list->insert(Token(line, res));
}

/* sign = part of number | operation */
void Lexer::
sign(TokenList *list)
{
	push();
	get();
	if (isdigit(ch)) {
		number(list);
	} else if (ispunct(ch)) {
		punctuation(list);
	} else {
		OpType type = buf[0] == '+' ? ADD : SUB;
		list->insert(Token(line, type));
		unget();
	}
}

/* word = statement | data type | identifier */
void Lexer::
word(TokenList *list)
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
	type = look(lexStatement);
	if (type >= 0) {
		list->insert(Token(line, static_cast<StatementType>(type)));
		return;
	}
	type = look(lexDataType);
	if (type >= 0) {
		list->insert(Token(line, static_cast<DataType>(type)));
		return;
	}
	list->insert(Token(line, buf));
}

void Lexer::
punctuation(TokenList *list)
{
	int type;
	do {
		push();
		get();
	} while(ispunct(ch) && !strchr(delimiters, ch));
	unget();
	end();
	type = look(lexOperation);
	if (type >= 0) {
		list->insert(Token(line, static_cast<OpType>(type)));
		return;
	}
	type = look(lexSpecialSymbol);
	if (type >= 0) {
		list->insert(Token(line, static_cast<TokenType>(type)));
		return;
	}
	errx(EXIT_FAILURE, err_invalid_op, line);
}

/* Check DFA to understand */
TokenList* Lexer::
analyze()
{
	TokenList *list = new TokenList;
	for (pos = 0;; pos = 0) {
		get();
		if (ch == EOF)
			break;
		if (isspace(ch)) {
			line += ch == '\n';
			continue;
		} else if (isdigit(ch))
			number(list);
		else if (ch == '+' || ch == '-')
			sign(list);
		else if (isalpha(ch))
			word(list);
		else if (ispunct(ch))
			punctuation(list);
		else
			errx(EXIT_FAILURE, err_invalid_char, line, ch);
		clear();
	}
	list->insert(Token(line, TOKEN_NULL));
	return list;
}

