#ifndef LEXEM_H
#define LEXEM_H

enum LexemType {
	LEX_NULL,
	BEGIN,
	END,
	LPARENTHESIS, RPARENTHESIS,
	LBRACKET, RBRACKET,
	EQUALSIGN,
	DATA_TYPE,
	CONST_INT,
	STATEMENT,
	IDENTIFIER,
	OPERATOR,
	SEMICOLON,
	COMMA
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

class Lexem {
	const LexemType type;
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
	Lexem(DataType dtype)
		: type(DATA_TYPE), value(dtype) {}
	~Lexem()
	{
		if (type == IDENTIFIER)
			delete [] value.id;
	}
	LexemType getType() const { return type; }
	const char *getIdentifier() const { return value.id; }
	DataType getDataType() const { return value.dtype; }
	int getInt() const { return value.v_int; }
	OpType getOpType() const { return value.optype; }
};

#endif
