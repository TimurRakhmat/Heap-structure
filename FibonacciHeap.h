#pragma once
#include"AHeap.h"
#include<map>


template<class TKey, class TValue>
class FibonacciHeap : public AHeap<TKey, TValue>
{
protected:
	int size = 0;
	class Node
	{
	public:
		TKey key;
		TValue val;
		Node* right = nullptr;
		Node* left = nullptr;
		Node* child = nullptr;
		int degree = 0;
		Node(TKey _key = 0, TValue _val = 0) : key(_key), val(_val) {};
	};
	Node* root = nullptr;

	Node* findMinNode()
	{
		if (size < 1)
			throw AHeap<TKey, TValue>::HeapException("FIND ERROR: EMPTY HEAP");
		map<int, Node*> mp;
		Node* tmp = root;
		do
		{
			Node* res = nullptr;
			if (mp.find(tmp->degree) != mp.end())
			{
				if (mp[tmp->degree] == tmp) // end;
					break;
				while (mp.find(tmp->degree) != mp.end())
				{
					Node* cgN = mp[tmp->degree];
					mp.erase(tmp->degree);
					Node* cgl = cgN->left, * cgr = cgN->right;
					cgr->left = cgl;
					cgl->right = cgr;
					cgl = tmp->left;
					cgr = tmp->right;
					bool rend = true;
					if (cgr == tmp)
						rend = false;
					res = merge_two(tmp, cgN);
					if (rend)
					{
						cgl->right = res;
						cgr->left = res;
						res->left = cgl;
						res->right = cgr;
					}
					else
					{
						res->left = res;
						res->right = res;
					}
					tmp = res;
					if (this->cmp->compare(root->key, tmp->key) > 0)
					{
						root = tmp;
					}
				}
				mp[tmp->degree] = tmp;
				tmp = tmp->right;
			}
			else
			{
				if (this->cmp->compare(root->key, tmp->key) > 0)
				{
					root = tmp;
				}
				mp[tmp->degree] = tmp;
				tmp = tmp->right;
			}
		} while (tmp != root);
	}

	FibonacciHeap(const Compare<TKey>* cmp, Node* nd, int _size) :AHeap<TKey, TValue>(cmp)
	{
		root = nd;
		size = _size;
	}

	Node* merge_two(Node* nd1, Node* nd2)
	{
		int res = this->cmp->compare(nd1->key, nd2->key);
		if (res > 0)
		{
			nd1->right = nd1;
			nd1->left = nd1;
			nd2->degree++;
			if (nd2->child) {
				nd1->right = nd2->child;
				nd1->left = nd2->child->left;
				nd1->left->right = nd1;
				nd1->right->left = nd1;
			}
			else
			{
				nd2->child = nd1;
			}
			return nd2;
		}
		nd2->right = nd2;
		nd2->left = nd2;
		nd1->degree++;
		if (nd1->child) {
			nd2->right = nd1->child;
			nd2->left = nd1->child->left;
			nd2->left->right = nd2;
			nd2->right->left = nd2;
		}
		else
		{
			nd1->child = nd2;
		}
		return nd1;
	}

	void merge_bin(FibonacciHeap<TKey, TValue>* heap)
	{
		if (!root)
		{
			root = heap->root;
			return;
		}
		if (!heap->root)
		{
			return;
		}
		root->left->right = heap->root;
		heap->root->left->right = root;

		Node* tmp = heap->root->left;
		heap->root->left = root->left;
		root->left = tmp;
	}

public:
	FibonacciHeap(const Compare<TKey>* cmp) :AHeap<TKey, TValue>(cmp)
	{
		size = 0;
	}

	void insert(TKey _key, TValue _val)
	{
		Node* tmp = new Node(_key, _val);
		size++;
		if (size == 1)
		{
			root = tmp;
			tmp->right = tmp;
			tmp->left = tmp;
			return;
		}
		tmp->right = root->right;
		tmp->right->left = tmp;
		root->right = tmp;
		tmp->left = root;
		if (this->cmp->compare(root->key, _key) > 0)
		{
			root = tmp;
		}
	}

	void removeMin()
	{
		if (size < 0)
			throw AHeap<TKey, TValue>::HeapException("REMOVE ERROR: EMPTY HEAP");

		FibonacciHeap* h1 = new FibonacciHeap(this->cmp, root->child, (1 << root->degree) - 1);
		Node* tmpl = root->left, *tmpr = root->right;
		tmpl->right = tmpr;
		tmpr->left = tmpl;
		if (root->right == root)
			tmpr = nullptr;
		delete root;
		root = nullptr;
		if (tmpr)
			root = tmpr;
		merge_bin(h1);
		size--;
		if (size > 0)
			findMinNode();
		h1->size = 0;
		delete h1;
	}

	TValue& findMin()
	{
		return root->val;
	}

	bool isEmpty()
	{
		if (size > 0)
			return true;
		else
			return false;
	}


	void merge(AHeap<TKey, TValue>* heap) {
		while (size > 0)
		{
			heap->insert(root->key, root->val);
			removeMin();
		}
	}

	~FibonacciHeap()
	{
		while (size > 0)
			removeMin();
	}
};
