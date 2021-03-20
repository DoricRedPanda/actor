#ifndef LEXER_H
#define LEXER_H
#include <string.h>
#include <stdio.h>
#include "lexem.h"
#include "list.h"

#define MAX_LEXEM_LENGTH 80

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
	void delimiter(List<Lexem> &list);
	void number(List<Lexem> &l);
	void word(List<Lexem> &l);
	void sign(List<Lexem> &l);
	void ident(List<Lexem> &l);
	void equalsign(List<Lexem> &l);
	void operation(List<Lexem> &l);
public:
	Lexer(FILE *file)
		: file(file), pos(0) {}
	List<Lexem> run();
};

#endif
