#ifndef TOKEN_H
#define TOKEN_H

#include "list.h"


enum TokenType {
	BEGIN, END,
	LBRACKET, RBRACKET,
	LPARENTHESIS, RPARENTHESIS,
	COMMA,
	EQUALSIGN,
	TWO_SPOT,
	SEMICOLON,
	TOKEN_NULL,
	DATA_TYPE,
	CONST_INT,
	STATEMENT,
	IDENTIFIER,
	OPERATOR
};

enum DataType {
	INT
};

enum OpType {
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
	NOT, BITNOT,
	// Following operations checked at the stage of syntax analysys
	NEG,
	DEREFERENCE,
	CALL,
	LParentOp
};

enum StatementType {
	GOTO,
	IF,
	ELSE,
	WHILE,
	READ,
	WRITE
};

class Token {
	const TokenType type;
	int line;
	union Value {
		const OpType optype;
		const DataType dtype;
		const StatementType stype;
		const int v_int;
		const char *id;
		Value() {}
		Value(OpType op) : optype(op) {}
		Value(DataType dtype) : dtype(dtype) {}
		Value(StatementType stype) : stype(stype) {}
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
	Token(const Token &t)
		: type(t.type), line(t.line), value(t.value)
	{
		if (type == IDENTIFIER)
			value.id = dupIdentifier(t.value.id);
	}

	Token(int line, TokenType type)
		: type(type), line(line) {}

	Token(int line, const char *id)
		: type(IDENTIFIER), line(line), value(dupIdentifier(id)) {}

	Token(int line, OpType optype)
		: type(OPERATOR), line(line), value(optype) {}

	Token(int line, int num)
		: type(CONST_INT), line(line), value(num) {}

	Token(int line, StatementType stype)
		: type(STATEMENT), line(line), value(stype) {}

	Token(int line, DataType dtype)
		: type(DATA_TYPE), line(line), value(dtype) {}

	~Token()
	{
		if (type == IDENTIFIER)
			delete [] value.id;
	}

	TokenType getType() const { return type; }
	int getLineNumber() const { return line; }
	const char *getIdentifier() const { return value.id; }
	DataType getDataType() const { return value.dtype; }
	int getInt() const { return value.v_int; }
	OpType getOpType() const { return value.optype; }
};


typedef List<Token> TokenList;

#endif
