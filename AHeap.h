#pragma once
#include <exception>
#include <iostream>
#include <string>
using namespace std;

template <class T>
class Compare
{
public:
	virtual ~Compare() {}
	virtual int compare(const T& left, const T& right) const = 0;
};

class IntCompare : public Compare<int>
{
public:
	IntCompare() {}
	int compare(const int& left, const int& right) const {
		if (left > right)
			return left - right;
		if (right > left)
			return left - right;
		return 0;
	}
};

template<class TKey, class TValue>
class AHeap
{
protected:
	const Compare <TKey>* cmp;
public:
	AHeap(const Compare <TKey>* _cmp) : cmp(_cmp) {}
	virtual void insert(TKey key, TValue val) = 0;
	virtual TValue& findMin() = 0;
	virtual void removeMin() = 0;
	virtual bool isEmpty() = 0;
	virtual void merge(AHeap<TKey, TValue>* heap) = 0;
	virtual ~AHeap() {};

	class HeapException : public exception
	{
	private:
		string error_text;

	public:
		HeapException(const string& _error_text) {
			error_text = _error_text;
		}
		const char* what() const noexcept {
			return error_text.c_str();
		}
	};
};