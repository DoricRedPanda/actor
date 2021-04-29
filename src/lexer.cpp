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
	";",
	NULL
};

static const char *lexBaseType[] = {
	"int",
	"void",
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
	"return",
	NULL
};

static const char err_wordlen[] = "\t%d\t|\tWord is too long";
static const char err_int_suffix[] = "\t%d\t|\tInvalid suffix on integer constant";
static const char err_invalid_char[] = "\t%d\t|\tUnexpected character %c";
static const char err_op_suffix[] = "\t%d\t|\tInvalid suffix on operation";
static const char err_invalid_op[] = "\t%d\t|\t Bad lexeme. Unrecognized operator";
static const char err_eof[] = "\t%d\t|\tUnexpected end of file";
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
	} else if (ispunct(ch) && !strchr(delimiters, ch)) {
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
	type = look(lexBaseType);
	if (type >= 0) {
		list->insert(Token(line, static_cast<BaseType>(type)));
		return;
	}
	if (ch == ':') {
		list->insert(Token(line, LABEL_DEF, buf));
		get();
	} else {
		list->insert(Token(line, IDENTIFIER, buf));
	}

}

void Lexer::
comments()
{
	if (ch != '*')
		return;
	if (buf[pos - 1] != '/')
		return;
	buf[pos - 1] = '\0';
	bool flag = false;
	for (;;) {
		get();
		if (ch == '\n')
			line++;
		else if (flag && ch == '/')
			break;
		else if (ch == EOF)
			errx(EXIT_FAILURE, err_eof, line);
		flag = ch == '*';
	}
	get();
}

void Lexer::
punctuation(TokenList *list)
{
	int type;
	do {
		push();
		get();
	} while(ispunct(ch) && !strchr(delimiters, ch));
	end();
	comments();
	unget();
	if (buf[0] == '\0')
		return;
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
	return list;
}

