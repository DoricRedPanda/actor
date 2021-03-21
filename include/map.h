#ifndef MAP_H
#define MAP_H
#include <stddef.h>
/*
 * red-black trees properties:  https://en.wikipedia.org/wiki/Rbtree
 *
 *  1) A node is either red or black
 *  2) The root is black
 *  3) All leaves (NULL) are black
 *  4) Both children of every red node are black
 *  5) Every simple path from root to leaves contains the same number
 *     of black nodes.
 *
 *  4 and 5 give the O(log n) guarantee, since 4 implies you cannot have two
 *  consecutive red nodes in a path and every red node is therefore followed by
 *  a black. So if B is the number of black nodes on every simple path (as per
 *  5), then the longest possible path due to 4 is 2B.
 *
 *  We shall indicate color with case, where black nodes are uppercase and red
 *  nodes will be lowercase. Unknown color nodes shall be drawn as red within
 *  parentheses and have some accompanying text comment.
 */
enum Color {RED, BLACK};

template<class Key, class T> struct Node {
	Color color;
	const Key key;
	T item;
	Node *left, *right, *parent;

	Node(const Key &k, const T &it, Node *prnt = NULL)
		: color(RED), key(k), item(it)
	{
		left = right = NULL;
		parent = prnt;
	}
};

template<class Key, class T, int compare(const Key, const Key)>
class Map {
	Node<Key, T> *root;
protected:
	void leftRotate(Node<Key, T> *node)
	{
		Node<Key, T> *head = node->right;
		node->right = head->left;
		if (head->left)
			head->left->parent = node->right;
		head->left = node;
		head->parent = node->parent;
		if (node->parent) {
			if (node == node->parent->left)
				node->parent->left = head;
			else
				node->parent->right = head;
		}
		node->parent = head;
	}

	void rightRotate(Node<Key, T> *node)
	{
		Node<Key, T> *head = node->left;
		node->left = head->right;
		if (head->right)
			head->right->parent = node->left;
		head->right = node;
		head->parent = node->parent;
		if (node->parent) {
			if (node == node->parent->left)
				node->parent->left = head;
			else
				node->parent->right = head;
		}
		node->parent = head;
	}

	Node<Key, T> *getNode(const Key &key) const
	{
		int cmp;
		Node<Key, T> *node = root;
		while (node) {
			cmp = compare(node->key, key);
			if (cmp > 0)
				node = node->left;
			else if (cmp < 0)
				node = node->right;
			else
				return node;
		}
		return NULL;
	}

	void rebalance(Node<Key, T> *node)
	{
		Node<Key, T> *gparent, *parent = node->parent, *uncle;
		while (node->color == RED && parent && parent->color == RED) {
			gparent = parent->parent; // Not NULL, because parent is red.
			uncle = gparent->right;
			if (parent != uncle) { //parent == gparent->left
				if (uncle && uncle->color == RED) { // Red Uncle
					/*
					 * Case 1 - node's uncle is red (color flips).
					 *
					 *       G            g
					 *      / \          / \
					 *     p   u  -->   P   U
					 *    /            /
					 *   n            n
					 *
					 * However, since g's parent might be red, and
					 * 4) does not allow this, we need to recurse
					 * at g.
					 */
					uncle->color = BLACK;
					parent->color = BLACK;
					gparent->color = RED;
					node = gparent;
					parent = node->parent;
					continue;
				}
				if (node == parent->right) {
					/*
					 * Case 2 - node's uncle is black and node is
					 * the parent's right child (left rotate at parent).
					 *
					 *      G             G
					 *     / \           / \
					 *    p   U  -->    n   U
					 *     \           /
					 *      n         p
					 *
					 * This still leaves us in violation of 4), the
					 * continuation into Case 3 will fix that.
					 */
					node = parent;
					leftRotate(node);
					parent = node->parent;
				}
				/*
				 * Case 3 - node's uncle is black and node is
				 * the parent's left child (right rotate at gparent).
				 *
				 *        G           P
				 *       / \         / \
				 *      p   U  -->  n   g
				 *     /                 \
				 *    n                   U
				 */
				parent->color = BLACK;
				gparent->color = RED;
				rightRotate(gparent);
				break;
			} else { // parent == gparent->right
				uncle = gparent->left;
				if (uncle && uncle->color == RED) { /* Case 1 - color flips */
					uncle->color = BLACK;
					parent->color = BLACK;
					gparent->color = RED;
					node = gparent;
					parent = node->parent;
					continue;
				}
				if (node == parent->left) { /* Case 2 - right rotate at parent */
					node = parent;
					rightRotate(node);
					parent = node->parent;
				}
				/* Case 3 - left rotate at gparent */
				parent->color = BLACK;
				gparent->color = RED;
				leftRotate(gparent);
				break;
			}
		}
		if (root->parent)
			root = root->parent;
		root->color = BLACK;
	}

	void clear(Node<Key, T> *node)
	{
		if (!node)
			return;
		clear(node->left);
		clear(node->right);
		delete node;
	}
public:
	Map() : root(NULL) {}
	~Map() { clear(root); }

	T *find(const Key &key) const
	{
		Node<Key, T> *node = getNode(key);
		return node ? &node->item : NULL;
	}
	T &operator[](const Key &key) const
	{
		return *find(key);
	}

	int insert(const Key &key, const T &item)
	{
		Node<Key, T> **link = &root, *parent = NULL;
		int cmp;
		while (*link)
		{
			parent = *link;
			cmp = compare((*link)->key, key);
			if (cmp > 0)
				link = &((*link)->left);
			else if (cmp < 0)
				link = &((*link)->right);
			else
				return -1;
		}
		*link = new Node<Key, T>(key, item, parent);
		rebalance(*link);
		return 0;
	}
};
#endif
