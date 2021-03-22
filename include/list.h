#ifndef LIST_H
#define LIST_H

#include <stddef.h>

template<class T> class List {
	template<class Type> struct Node {
		Type item;
		Node<Type> *next;
		Node<Type>(const Type &item) : item(item), next(NULL) {}
	};

	template<class Type> struct Node<Type*> {
		Type *item;
		Node<Type*> *next;
		Node<Type*>(Type *item) : item(item), next(NULL) {}
		~Node() { delete item; }
	private:
		Node(const Node &lnode) {}
	};

	Node<T> *head;
	Node<T> *cur;
	Node<T> *tail;
public:
	List() : head(NULL), cur(NULL), tail(NULL) {}
	~List() { clear(); }

	void insert(const T &item)
	{
		if (!tail) {
			head = cur = tail = new Node<T>(item);
			return;
		}
		tail->next = new Node<T>(item);
		tail = tail->next;
	}

	void clear()
	{
		Node<T> *ptr;
		while (head) {
			ptr = head;
			head = head->next;
			delete ptr;
		}
	}

	void setPos(Node<T> *ptr) { cur = ptr; }

	T* next()
	{
		if (!cur)
			return NULL;
		T *res = &(cur->item);
		cur = cur->next;
		return res;
	}

	void begin()
	{
		cur = head;
	}
};

#endif
