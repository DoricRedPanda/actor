#ifndef TOKEN_H
#define TOKEN_H

#include "list.h"


enum TokenType {
	BEGIN, END,
	LBRACKET, RBRACKET,
	LPARENTHESIS, RPARENTHESIS,
	COMMA,
	EQUALSIGN,
	SEMICOLON,
	TOKEN_NULL,
	DATA_TYPE,
	CONST_INT,
	STATEMENT,
	IDENTIFIER,
	OPERATOR,
	LABEL_DEF
};

enum BaseType {
	INT,
	LABEL,
	POINTER,
	ARRAY
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
	CALL
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
		const BaseType dtype;
		const StatementType stype;
		const int v_int;
		const char *id;
		Value() {}
		Value(OpType op) : optype(op) {}
		Value(BaseType dtype) : dtype(dtype) {}
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
		if (type == IDENTIFIER || type == LABEL_DEF)
			value.id = dupIdentifier(t.value.id);
	}

	Token(int line, TokenType type)
		: type(type), line(line) {}

	Token(int line, TokenType type, const char *id)
		: type(type), line(line), value(dupIdentifier(id)) {}

	Token(int line, OpType optype)
		: type(OPERATOR), line(line), value(optype) {}

	Token(int line, int num)
		: type(CONST_INT), line(line), value(num) {}

	Token(int line, StatementType stype)
		: type(STATEMENT), line(line), value(stype) {}

	Token(int line, BaseType dtype)
		: type(DATA_TYPE), line(line), value(dtype) {}

	~Token()
	{
		if (type == IDENTIFIER || type == LABEL_DEF)
			delete [] value.id;
	}

	TokenType getType() const { return type; }
	int getPos() const { return line; }
	const char *getIdentifier() const { return value.id; }
	BaseType getBaseType() const { return value.dtype; }
	StatementType getStatementType() const { return value.stype; }
	int getInt() const { return value.v_int; }
	OpType getOpType() const { return value.optype; }
};


typedef List<Token> TokenList;

#endif
