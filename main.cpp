#include<iostream>
#include"AHeap.h"
#include"BinaryHeap.h"
#include"BinomialHeap.h"

using namespace std;

int main()
{
	Compare<int>* cmp = new IntCompare;
	AHeap<int, int>*h = new BinomialHeap<int, int>(cmp);
	BinomialHeap<int, int> b(cmp);
	BinomialHeap<int, int> c(cmp);
	for (int i = 0; i < 20; i++)
	{
		b.insert(i, i);
	}

	for (int i = 20; i < 340; i++)
	{
		c.insert(i, i);
	}

	b.merge(&c);

	
	for (int i = 0; i < 340; i++)
	{
		cout << c.findMin() << endl;
		c.removeMin();
	}

	//cout << b.findMin() << endl;
	delete cmp;
	delete h;
}