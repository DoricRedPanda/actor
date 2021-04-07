#ifndef POLIZ_H
#define POLIZ_H
#include <stdint.h>
#include <stdio.h>
#include "stack.h"
#include "list.h"

typedef Stack<intptr_t> SStack;

class PolizItem {
protected:
	PolizItem() {}
public:
	virtual void eval(SStack &stack, List<PolizItem*> *ptrPoliz) = 0; /* TODO: unused arguments */
	virtual ~PolizItem() {}
};

typedef List<PolizItem*> Poliz;

class PolizOpGo: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Constant: public PolizItem {
protected:
	Constant() {}
public:
	virtual void eval(SStack &stack, Poliz *ptrPoliz) = 0;
	~Constant() {}
};

template<class T> class GenericConstant : public Constant {
	T value;
public:
	GenericConstant(T value)
		: value(value) {}
	T get() const { return value; }
	void set(T v) { value = v; }
	void eval(SStack &stack, Poliz *ptrPoliz)
	{
		stack.push((intptr_t) value);
	}
};

typedef GenericConstant<intptr_t> ConstInt;
typedef GenericConstant<Node<PolizItem*>*> Label;

class Variable: public PolizItem {
	int value;
public:
	Variable(int v) : value(v) {}
	void eval(SStack &stack, Poliz *ptrPoliz) { stack.getLength(); }
	void *getPointer() { return &value; }
};

class Instruction: public PolizItem {
public:
	virtual void eval(SStack &stack, Poliz *ptrPoliz) = 0;
	~Instruction() {}
};

class Inst_mov: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_or: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_and: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_bitor: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_xor: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_bitand: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_eq: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_neq: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_leq: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_lt: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_geq: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_gt: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_shl: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_shr: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_add: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_sub: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_mul: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_div: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_mod: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_not: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_bitnot: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_neg: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Inst_dereference: public Instruction {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};


#endif
