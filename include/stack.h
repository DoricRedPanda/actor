#ifndef STACK_H
#define STACK_H
#include <err.h>
#include <stdlib.h>

template <class T> class Stack {
protected:
	T *item;
	size_t size;
	size_t tail;
	void expand(size_t additional)
	{
		size += additional;
		item = (T *) realloc(item, size * sizeof(T));
		if (!item)
			err(EXIT_FAILURE, "Failed to resize Stack!");
	}
public:
	Stack(size_t size)
		: size(size), tail(0)
	{
		item = (T *) malloc(size * sizeof(T));
		if (!item)
			err(EXIT_FAILURE, "Failed to create Stack!");
	}

	void push(const T &value)
	{
		item[tail] = value;
		tail++;
		if (tail == size)
			this->expand(size);
	}

	T pop() { return item[--tail]; }
	T front() const { return item[tail - 1]; }
	size_t getLength() const { return tail; }
	~Stack() { free(item); }
};
#endif
