#ifndef POLIZ_H
#define POLIZ_H
#include <stdint.h>
#include <stdio.h>
#include "stack.h"
#include "vector.h"

typedef Stack<intptr_t> SStack;

class PolizItem {
protected:
	PolizItem() {}
public:
	virtual void eval(SStack &stack) = 0;
	virtual ~PolizItem() {}
};

class Constant: public PolizItem {
protected:
	Constant() {}
public:
	virtual void eval(SStack &stack) = 0;
	~Constant() {}
};

template<class T> class GenericConstant : public Constant {
	T value;
public:
	GenericConstant(T value)
		: value(value) {}
	T get() const { return value; }
	void eval(SStack &stack)
	{
		stack.push((intptr_t) value);
	}
};

typedef GenericConstant<intptr_t> ConstInt;
typedef GenericConstant<PolizItem*> Label;

class Variable: public PolizItem {
	int value;
public:
	Variable(int v) : value(v) {}
	void eval(SStack &stack) { stack.getLength(); }
	void *getPointer() { return &value; }
};

class Instruction: public PolizItem {
public:
	void eval(SStack &stack) = 0;
	~Instruction() {}
};

class Inst_mov: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_add: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_sub: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_mul: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_div: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_dereference: public Instruction {
public:
	void eval(SStack &stack);
};

#endif
