#ifndef LEXER_H
#define LEXER_H
#include <string.h>
#include <stdio.h>
#include "token.h"
#include "list.h"

#define MAX_LEXEME_LENGTH 80

class Lexer {
	FILE *file;
	char buf[MAX_LEXEME_LENGTH + 1];
	int pos;
	int ch;
	int line;

	/* utility */
	void get() { ch = fgetc(file); }
	void unget() { ungetc(ch, file); }
	void push();
	void end() { buf[pos] = '\0'; }
	void clear() { pos = 0; }
	int look(const char *table[]) const;

	/* Following functions represent states of DFA.
	 * On success lexem is added to the list.
	 */
	void delimiter(TokenList *list);
	void number(TokenList *list);
	void word(TokenList *list);
	void sign(TokenList *list);
	void ident(TokenList *list);
	void punctuation(TokenList *list);
public:
	Lexer(FILE *file)
		: file(file), pos(0), line(0) {}
	TokenList* analyze();
};

#endif
