#ifndef LEXER_H
#define LEXER_H
#include <string.h>
#include <stdio.h>
#include "lexem.h"
#include "list.h"

#define MAX_LEXEM_LENGTH 80

typedef List<Lexem> LexemList;

class Lexer {
	FILE *file;
	char buf[MAX_LEXEM_LENGTH + 1];
	int pos;
	int ch;
	void get() { ch = fgetc(file); }
	void unget() { ungetc(ch, file); }
	void push();
	void end() { buf[pos] = '\0'; }
	void clear() { pos = 0; }
	int look(const char *table[]) const;

	/* Following functions represent states of DFA.
	 * On success lexem is added to the list.
	 */
	void delimiter(LexemList *list);
	void number(LexemList *list);
	void word(LexemList *list);
	void sign(LexemList *list);
	void ident(LexemList *list);
	void equalsign(LexemList *list);
	void operation(LexemList *list);
public:
	Lexer(FILE *file)
		: file(file), pos(0) {}
	LexemList* analyze();
};

#endif
