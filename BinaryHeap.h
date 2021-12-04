#pragma once

#include"AHeap.h"

template<class TKey, class TValue>
class BinaryHeap : public AHeap<TKey, TValue>
{
protected:
	int static_size = 200;
	int size = 0;
	class Node
	{
	public:
		TKey key;
		TValue val;
		Node(TKey _key = 0, TValue _val = 0) : key(_key), val(_val) {};
	};
	Node* h = nullptr;

	void heapify(int i) {
		int left, right;
		Node temp;
		left = 2 * i + 1;
		right = 2 * i + 2;
		if (left < size) {
			if (AHeap<TKey, TValue>::cmp->compare(h[i].key, h[left].key) > 0) {
				temp = h[i];
				h[i] = h[left];
				h[left] = temp;
				heapify(left);
			}
		}
		if (right < size) {
			if (AHeap<TKey, TValue>::cmp->compare(h[i].key, h[right].key) > 0) {
				temp = h[i];
				h[i] = h[right];
				h[right] = temp;
				heapify(right);
			}
		}
	}

	void append_memmory()
	{
		Node* tmp = h;
		static_size *= 2;
		h = new Node[static_size];
		static_size *= 2;
		for (int i = 0; i < size; i++)
			h[i] = tmp[i];
		delete tmp;
	}

public:
	BinaryHeap(const Compare<TKey>* cmp) :AHeap<TKey, TValue>(cmp)
	{
		size = 0;
		h = new Node[static_size];
	}

	void insert(TKey _key, TValue _val)
	{
		if (size == static_size)
			append_memmory();
		int i, parent;
		i = size;
		h[i].key = _key;
		h[i].val= _val;
		parent = (i - 1) / 2;
		while (parent >= 0 && i > 0) {
			int comp_res = AHeap<TKey, TValue>::cmp->compare(h[i].key, h[parent].key);
			// impossible to check with O(logn) need to check all elements (with O(n)
			/*if (comp_res == 0)
			{
				throw AHeap<TKey, TValue>::HeapException("INSERT ERROR: repeat key");
			}*/
			if (comp_res < 0) {
				Node temp = h[i];
				h[i] = h[parent];
				h[parent] = temp;
			}
			i = parent;
			parent = (i - 1) / 2;
		}
		size++;
	}

	void removeMin()
	{
		if (size < 1)
			throw AHeap<TKey, TValue>::HeapException("REMOVE ERROR: EMPTY HEAP");
		Node x;
		x = h[0];
		h[0] = h[size - 1];
		--size;
		heapify(0);
	}

	TValue& findMin()
	{
		if (size < 0)
			throw AHeap<TKey, TValue>::HeapException("REMOVE ERROR: EMPTY HEAP");
		return h[0].val;
	}

	bool isEmpty()
	{
		if (size > 0)
			return true;
		else
			return false;
	}


	void merge(AHeap<TKey, TValue>* heap) {
		while (size != 0)
		{
			heap->insert(h[0].key, h[0].val);
			this->removeMin();
		}
	}

	~BinaryHeap()
	{
		if (h)
			delete h;
	}
};
