#ifndef LEXEM_H
#define LEXEM_H

enum LexemType {
	CONST_INT,
	STATEMENT,
	IDENTIFIER,
	OPERATOR,
	SEMICOLON,
	COMMA
};

enum OpType {
	LBRACKET, RBRACKET,
	UPLUS, UMINUS,
	OR,
	AND,
	BITOR,
	XOR,
	BITAND,
	EQ, NEQ,
	LEQ, LT, GEQ, GT,
	SHL, SHR,
	ADD, SUB,
	MUL, DIV, MOD,
	NOT_OPERATOR
};

enum StatementType {
	BEGIN,
	END,
	ASSIGN,
	TYPE_INT,
	GOTO,
	IF,
	ELSE,
	WHILE,
	READ,
	WRITE,
	TRUE,
	FALSE
};

class Lexem {
	const LexemType type;
	union Value {
		const OpType optype;
		const int v_int;
		const char *id;
		Value() {}
		Value(OpType op) : optype(op) {}
		Value(int i) : v_int(i) {}
		Value(const char *id) : id(id) {}
	} value;

	inline const char* dupIdentifier(const char *str)
	{
		size_t size = strlen(str) + 1;
		char *tmp = new char[size];
		memcpy(tmp, str, size);
		return tmp;
	}
public:
	Lexem(const Lexem &lex)
		: type(lex.type), value(lex.value)
	{
		if (type == IDENTIFIER)
			value.id = dupIdentifier(lex.value.id);
	}
	Lexem(LexemType type)
		: type(type) {}
	Lexem(const char *id)
		: type(IDENTIFIER), value(dupIdentifier(id)) {}
	Lexem(OpType optype)
		: type(OPERATOR), value(optype) {}
	Lexem(int num)
		: type(CONST_INT), value(num) {}
	Lexem(StatementType stype)
		: type(STATEMENT), value(stype) {}
	~Lexem()
	{
		if (type == IDENTIFIER)
			delete [] value.id;
	}
	LexemType getType() const { return type; }
	const char *getIdentifier() const { return value.id; }
};

#endif
