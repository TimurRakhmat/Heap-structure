#pragma once
#include <cstdlib> // для функций rand() и srand()
#include <ctime> 
#include <string>
#include <vector>
#include"..\..\Logger.h"
#include"..\..\BinaryHeap.h"
#include"..\..\FibonacciHeap.h"
#include"..\..\BinomialHeap.h"

class Bank
{
protected:
	int n = 5; // count of departament
	vector<int> pers = { 4, 8, 2, 3, 9 }; // count of personal in each departament
	static const int t = 15; // time delay
	static const int td = 5; // delta time
	int lmax = 2;
	int current_l = 0;
	int l = 1; // in %

	class MSG
	{
	private:
		
	public:
		int u;
		int h;
		int min;
		int a;
		int c;
		string text;
		MSG(int u_, int h_, int min_, int a_, int c_, string& text_) :u(u_), h(h_), min(min_), a(a_), c(c_), text(text_) {}
	};

	class GeneratorMSG
	{
	public:
		vector<string> first = { "Need", "Should", "Please", "Return", "Pay" };
		vector<string> second = { "", "procent", "obligation", "credit", "price" };
		vector<string> third = { "rub", "us", "eu", "krn", "uan" };
		vector<MSG*> generate_msg(int h, int min, int count);
	};

	GeneratorMSG generator;

	class MSGCmp : public Compare<MSG*>
	{
	public:
		MSGCmp() {}
		int compare(MSG* const& left, MSG* const& right) const {
			if (left->u == right->u and left->h == right->h and left->min == right->min)
				return 0;
			if (left->u > right->u)
				return left->u - right->u;
			if (right->h > left->h)
				return left->h - right->h;
			if (right->min > left->min)
				return left->min - right->min;
			return -1;
		}
	};

	Compare<MSG*>* cmp = nullptr;

	class Departament
	{
	public:
		class Slave
		{
		public:
			int work_time = 0;
			int dinner_time = 0;
			int task_time = 0;
			bool free = true, dinner = false, have_dinner = false, finished_work = false;
			int id;
			Logger* lg_s;
			Slave(Logger* lg_, int h, int min, int id_);
			bool isFree() { return free; }
			void setTask(MSG* msg, int h, int min);
			void try_dinner(int h, int min);
			void stop_dinner(int h, int min);
			void step(int h, int min);
		};
	public:
		vector<Slave*> ps;
		int personal_count;
		int personal_current_count = 0;
		bool free = true;
		int extra_time = 0;
		int num;
		int msg_count = 0;
		Logger lg;
		int h = 8;
		int min = 0;
		AHeap<MSG*, MSG*>* msg_heap;
		void add_msg(Bank::MSG* msg);
		void nextStep();
		bool isFree() { return free; }
		double getCapacity();
		Departament(int num_, Compare<Bank::MSG*>* cmp, int n = 5);
		~Departament();
		void setPause(int num_of_dep);
	};
	vector<Departament*> deps;
	Departament* find_free();
public:
	void init();
	void addMSG(MSG* msg);
	void start();
	~Bank();
};

