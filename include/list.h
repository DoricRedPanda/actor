#ifndef LIST_H
#define LIST_H

template<class T> struct LNode {
	T item;
	LNode<T> *next;
	LNode<T>(const T &item) : item(item), next(NULL) {}
};

template<class T> class List {
	LNode<T> *head;
	LNode<T> *cur;
	LNode<T> *tail;
public:
	List() : head(NULL), cur(NULL), tail(NULL) {}
	~List() { clear(); }

	void insert(const T &item)
	{
		if (!tail) {
			head = cur = tail = new LNode<T>(item);
			return;
		}
		tail->next = new LNode<T>(item);
		tail = tail->next;
	}

	void clear()
	{
		LNode<T> *ptr;
		while (head) {
			ptr = head;
			head = head->next;
			delete ptr;
		}
	}

	void setPos(LNode<T> *ptr) { cur = ptr; }

	T* next()
	{
		if (!cur)
			return NULL;
		T *res = &(cur->item);
		cur = cur->next;
		return res;
	}
};

#endif
