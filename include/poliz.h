#ifndef POLIZ_H
#define POLIZ_H
#include <stdint.h>
#include <stdio.h>
#include "stack.h"
#include "list.h"

class SStack: public Stack<intptr_t> {
	size_t bottomPointer;
public:
	SStack(size_t size) : Stack(size), bottomPointer(0) {}
	intptr_t& operator[](intptr_t i) { return item[bottomPointer + i]; }
	void setBP(size_t bp) { bottomPointer = bp; }
	intptr_t getSP() { return tail; }
	intptr_t getBP() { return bottomPointer; }
	void createFrame()
	{
		this->push(getBP());
		this->setBP(getSP() - 1);
	}

	void destroyFrame()
	{
		tail = bottomPointer;
		bottomPointer = item[bottomPointer];
	}

	void reserve(size_t count) {
		if (tail + count >= size)
			expand(tail + count - size);
		tail += count;
	}

	void slice(size_t count) { tail -= count; }
};


/* TODO virtual methods probably cripple performance. Need research
 * Also, casting may prevent optimizations.
 */

class PolizItem {
protected:
	PolizItem() {}
public:
	virtual void eval(SStack &stack, List<PolizItem*> *ptrPoliz) = 0;
	virtual ~PolizItem() {}
};

typedef List<PolizItem*> Poliz;
typedef List<PolizItem*>::Node<PolizItem*> PolizItemNode;

class PolizExit: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class PolizOpGo: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class PolizOpGoFalse: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class SubprogramCall: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class ProcedureReturn: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class FunctionReturn: public PolizItem {
public:
	void eval(SStack &stack, Poliz *ptrPoliz);
};

class Instruction: public PolizItem {
public:
	virtual void eval(SStack &stack) = 0;
	void eval(SStack &stack, Poliz *ptrPoliz)
	{
		eval(stack);
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
	void eval(SStack &stack) {}
	void* getPointer() { return &value; }
};

class Array: public PolizItem {
	size_t size;
	void *array;
public:
	Array(size_t size) : size(size) {
		array = malloc(size);
		if (!array)
			err(EXIT_FAILURE, "Array allocation failed");
	}
	~Array() { free(array); }
	void eval(SStack &stack, Poliz *polizPtr) {}
	void* getPointer() { return array; }
};

class SubprogramAddress: public Instruction {
public:
	void eval(SStack &stack) {}
};

class Subprogram: public Instruction {
	size_t localVarCount;
public:
	Subprogram(size_t count) : localVarCount(count) {}
	void eval(SStack &stack);
};

class CallerRemovesArguments: public Instruction {
	size_t argNum;
public:
	CallerRemovesArguments(size_t count) : argNum(count) {}
	void eval(SStack &stack);
};

class Inst_print: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_mov: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_movStack: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_or: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_and: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_bitor: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_xor: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_bitand: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_eq: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_neq: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_leq: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_lt: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_geq: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_gt: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_shl: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_shr: public Instruction {
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

class Inst_mod: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_not: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_bitnot: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_neg: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_dereference: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_dereferenceStack: public Instruction {
public:
	void eval(SStack &stack);
};

class Inst_AddrPlus: public Instruction {
public:
	void eval(SStack &stack);
};

#endif
