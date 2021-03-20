#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

FILE*
init(int argc, char *argv[])
{
	FILE *file;
	if (argc != 2)
		errx(EXIT_FAILURE, "Wrong number of arguments!");
	file = fopen(argv[1], "r");
	if (!file)
		err(EXIT_FAILURE, "Can't open file");
	return file;
}

int
main(int argc, char *argv[])
{
	FILE *file;
	file = init(argc, argv);
	Lexer lexer(file);
	List<Lexem> list = lexer.run();
	Parser parser(list);
	parser.analyze();
	/*
	Interpreter interpreter(parser.run());
	interpreter.run();
	*/
	fclose(file);
	return 0;
}
