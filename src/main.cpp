#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

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

	int exit_code = Interpreter(file).run();

	fclose(file);
	return exit_code;
}
