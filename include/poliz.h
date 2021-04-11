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
	virtual void eval(SStack &stack, List<PolizItem*> *ptrPoliz) = 0;
	virtual ~PolizItem() {}
};

typedef List<PolizItem*> Poliz;
typedef List<PolizItem*>::Node<PolizItem*> PolizItemNode;

class PolizOpGo: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class PolizOpGoFalse: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Instruction: public PolizItem {
public:
	virtual void eval(SStack &stack) = 0;
	void eval(SStack &stack, Poliz *ptrPoliz)
	{
		eval(stack);
		ptrPoliz = ptrPoliz;
	}
	~Instruction() {}
};

template<class T> class GenericConstant : public Instruction {
	T value;
public:
	GenericConstant(T value)
		: value(value) {}
	T get() const { return value; }
	void set(T v) { value = v; }
	void eval(SStack &stack)
	{
		stack.push(reinterpret_cast<intptr_t>(value));
	}
};

typedef GenericConstant<intptr_t> ConstInt;
typedef GenericConstant<void*> IntAddress;
typedef GenericConstant<PolizItemNode*> Label;

class Variable: public Instruction {
	int value;
public:
	Variable(int v) : value(v) {}
	void eval(SStack &stack) { stack = stack; }
	void* getPointer() { return &value; }
};

class Inst_print: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_mov: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_or: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_and: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_bitor: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_xor: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_bitand: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_eq: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_neq: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_leq: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_lt: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_geq: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_gt: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_shl: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_shr: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_add: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_sub: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_mul: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_div: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_mod: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_not: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_bitnot: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_neg: public Instruction {
public:
	void eval(SStack &stackPoliz);
};

class Inst_dereference: public Instruction {
public:
	void eval(SStack &stackPoliz);
};


#endif
