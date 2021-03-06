#ifndef LIST_H
#define LIST_H

#include <stddef.h>

template<class T> class List {
public:
	template<class Type> struct Node {
		Type item;
		Node *next;
		Node(const Type &item) : item(item), next(NULL) {}
	};

	template<class Type> struct Node<Type*> {
	private:
		Node(const Node &lnode);
	public:
		Type *item;
		Node *next;
		Node(Type *item) : item(item), next(NULL) {}
		~Node() { delete item; }
	};
private:
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
	Node<T> *getPos() const { return cur; }
	Node<T> *getTail() const { return tail; }
	void begin() { cur = head; }
	void end() { cur = tail; }

	T& getCurItem() { return cur->item; }
	void step() { cur = cur->next; }
	T* next()
	{
		if (!cur)
			return NULL;
		T *res = &(cur->item);
		cur = cur->next;
		return res;
	}

	bool isEnding() const { return cur == NULL; }
};

#endif
