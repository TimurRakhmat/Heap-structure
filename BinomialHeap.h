#pragma once
#include"AHeap.h"

template<class TKey, class TValue>
class BinomialHeap : public AHeap<TKey, TValue>
{
protected:
	int size = 0;
	class Node
	{
	public:
		TKey key;
		TValue val;
		Node* right = nullptr;
		Node* child = nullptr;
		int degree = 0;
		Node(TKey _key = 0, TValue _val = 0) : key(_key), val(_val) {};
	};
	Node* root = nullptr, *rootm = nullptr;

	Node* findMinNode()
	{
		if (size < 1)
			throw AHeap<TKey, TValue>::HeapException("FIND ERROR: EMPTY HEAP");

		Node* tmp = root->right;
		rootm = root;
		while (tmp != root)
		{
			if (this->cmp->compare(rootm->key, tmp->key) > 0)
			{
				rootm = tmp;
			}
			tmp = tmp->right;
		}
	}

	BinomialHeap(const Compare<TKey>* cmp, Node* nd, int _size) :AHeap<TKey, TValue>(cmp)
	{
		root = nd;
		size = _size;
	}

	Node* merge_two(Node* nd1, Node* nd2)
	{
		nd1->right = nd1;
		nd2->right = nd2;
		int res = this->cmp->compare(nd1->key, nd2->key);
		if (res > 0)
		{
			nd2->degree++;
			if (nd2->child) {
				Node* tmp = nd2->child->right;
				while (tmp->right != nd2->child)
					tmp = tmp->right;
				tmp->right = nd1;
			}
			else
			{
				nd2->child = nd1;
			}
			nd1->right = nd2->child;
			return nd2;
		}
		
		nd1->degree++;
		if (nd1->child) {
			Node* tmp = nd1->child->right;
			while (tmp->right != nd1->child)
				tmp = tmp->right;
			tmp->right = nd2;
		}
		else
		{
			nd1->child = nd2;
		}
		nd2->right = nd1->child;
		return nd1;
	}

	Node* merge_bin(BinomialHeap<TKey, TValue>* heap)
	{
		if (!root)
			return heap->root;
		if (!heap->root)
			return root;
		Node* res = nullptr;
		Node* res_par = nullptr;
		Node* res_tmp = nullptr;
		Node* tmp = nullptr;
		bool e1 = true, e2 = true, e3 = true;

		Node* tmps = root, * tmpi = heap->root;

		do 
		{
			if (tmps->degree == tmpi->degree)
			{
				Node* h1 = tmps, * h2 = tmpi;
				tmps = tmps->right;
				tmpi = tmpi->right;

				if (tmps == root)
					e1 = false;
				if (tmpi == heap->root)
					e2 = false;

				tmp = merge_two(h1, h2);
				if (!res_tmp) {
					res = tmp;
					res_tmp = res;
				}
				else
				{
					if (tmp->degree == res_tmp->degree)
					{
						if (!e3)
						{
							res_par = res;
							while (res_par->right != res_tmp)
								res_par = res_par->right;
						}
						res_tmp = merge_two(res_tmp, tmp);
						if (!e3)
							res_par->right = res_tmp;
						if (e3)
							res = res_tmp;
					}
					else if (tmp->degree > res_tmp->degree)
					{
						res_tmp->right = tmp;
						res_tmp = res_tmp->right;
						e3 = false;
					}
				}
			}
			else
			{
				Node* h1 = nullptr;
				if (tmps->degree < tmpi->degree)
				{
					h1 = tmps;
					tmps = tmps->right;
					if (tmps == root)
						e1 = false;
				}
				else
				{
					h1 = tmpi;
					tmpi = tmpi->right;
					if (tmpi == heap->root)
						e2 = false;
				}

				if (!res_tmp) {
					res = h1;
					res_tmp = res;
				}
				else
				{
					if (h1->degree == res_tmp->degree)
					{
						if (!e3)
						{
							res_par = res;
							while (res_par->right != res_tmp)
								res_par = res_par->right;
						}
						res_tmp = merge_two(res_tmp, h1);
						if (!e3)
							res_par->right = res_tmp;
						if (e3)
							res = res_tmp;
					}
					else if (h1->degree > res_tmp->degree)
					{
						res_tmp->right = h1;
						res_tmp = res_tmp->right;
						e3 = false;
					}
				}
			}
		} while ((tmps != root and tmpi !=  heap->root) or (e1 and e2));

		
		do
		{
			if (!e1)
				break;
			Node* h1 = nullptr;
			h1 = tmps;
			tmps = tmps->right;

			if (!res_tmp) {
				res = h1;
				res_tmp = res;
			}
			else
			{
				if (h1->degree == res_tmp->degree)
				{
					if (!e3)
					{
						res_par = res;
						while (res_par->right != res_tmp)
							res_par = res_par->right;
					}
					res_tmp = merge_two(res_tmp, h1);
					if (!e3)
						res_par->right = res_tmp;
					if (e3)
						res = res_tmp;
				}
				else if (h1->degree > res_tmp->degree)
				{
					res_tmp->right = h1;
					res_tmp = res_tmp->right;
					e3 = false;
				}
			}
		}while (tmps != root);

		
		do
		{
			if (!e2)
				break;
			Node* h1 = nullptr;
			h1 = tmpi;
			tmpi = tmpi->right;

			if (!res_tmp) {
				res = h1;
				res_tmp = res;
			}
			else
			{
				if (h1->degree == res_tmp->degree)
				{
					if (!e3)
					{
						res_par = res;
						while (res_par->right != res_tmp)
							res_par = res_par->right;
					}
					res_tmp = merge_two(res_tmp, h1);
					if (!e3)
						res_par->right = res_tmp;
					if (e3)
						res = res_tmp;
				}
				else if (h1->degree > res_tmp->degree)
				{
					res_tmp->right = h1;
					res_tmp = res_tmp->right;
					e3 = false;
				}
			}
		}while (tmpi != heap->root);

		res_tmp->right = res;
		return res;
	}

	void rec_del(Node* _root)
	{
		if (_root->child) {
			while (_root->child->right != _root->child)
			{
				Node* tmp = _root->child->right;
				tmp = tmp->right;
				rec_del(_root->child->right);
				_root->child->right = tmp;
			}
			rec_del(_root->child);
		}
		delete _root;
	}

public:
	BinomialHeap(const Compare<TKey>* cmp) :AHeap<TKey, TValue>(cmp)
	{
		size = 0;
	}

	void insert(TKey _key, TValue _val)
	{	
		Node* tmp = new Node (_key, _val);
		tmp->right = tmp;
		BinomialHeap *h1 = new BinomialHeap(this->cmp, tmp, 1);
		root = merge_bin(h1);
		size++;
		findMinNode();
		h1->size = 0;
		delete h1;
		
	}

	void removeMin()
	{
		if (size < 0)
			throw AHeap<TKey, TValue>::HeapException("REMOVE ERROR: EMPTY HEAP");

		BinomialHeap *h1 = new BinomialHeap(this->cmp, rootm->child, (1 << rootm->degree) - 1);
		Node* tmp = root;
		while (tmp->right != rootm)
			tmp = tmp->right;
		tmp->right = rootm->right;
		if (rootm == root)
			root = tmp->right;
		if (root == rootm)
			root = nullptr;
		delete rootm;
		rootm = nullptr;
		root = merge_bin(h1);
		size--;
		if (size > 0)
			findMinNode();
		h1->size = 0;
		delete h1;
	}

	TValue& findMin()
	{
		return rootm->val;
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
			heap->insert(rootm->key, rootm->val);
			removeMin();
		}
	}

	~BinomialHeap()
	{
		if (size > 0)
		{
			while (root->right != root)
			{
				Node* tmp = root->right;
				root->right = tmp->right;
				rec_del(tmp);
			}
			rec_del(root);
			root = nullptr;
			rootm = nullptr;
			size = 0;
		}
	}
};
