#pragma once
#include <iostream>
#include <cstdlib> // для функций rand() и srand()
#include <ctime> 
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <cmath>
#include"..\Logger.h"
#include"..\BinaryHeap.h"
#include"..\FibonacciHeap.h"
#include"..\BinomialHeap.h"

using namespace std;

class Home
{
public:
	int n=0;
	int k=0;

	Logger* lt_lg = nullptr, *pl_lg = nullptr;

	class Time {
	public:
		int h;
		int min;
		int sec;
		string get_string();

		void set_string(string);
		int get_int_time();
		int get_int_time_sec();
		Time(string time_s);
		Time(int h_, int min_, int sec_ = 0);
	};

	class TimeCmp : public Compare<Time*>
	{
	public:
		TimeCmp() {}
		int compare(Time* const& left, Time* const& right) const {
			int t1 = left->get_int_time(), t2 = right->get_int_time();
			return t1 - t2;
		}
	};

	class People {
	public:
		int id;
		int weight;
		Time* tm = nullptr;
		Time* tstart = nullptr;
		Time* tend = nullptr;
		int start;
		int end;
		int oferflow_count = 0;
		set<int> neighbours;
		Logger* lg;
		string get_id_string();

		People(int id_, int weight_, int start_, int end_, Time* tm_, Logger* lg_);
		~People();
	};

	Compare<Time*>* cmp = nullptr;

	class Lift {
	public:
		int number;
		int m;
		int current_m = 0;
		int state = 0;
		int home_lvl = 0;
		int road_time = 0;
		Logger* lg;
		Compare<Time*>* tcmp = nullptr;

		int rest_time = 0;
		int work_time = 0;
		int lvl_count = 0;
		int total_weight = 0;
		int max_weight = 0;
		int overflow_count = 0;
		int dir = 0; // 1-up, -1-down, 0-wait;
		vector<bool> state_lvl;
		vector<vector<People*>> peopls;
		
		void step();
		void try_to_add_people(AHeap<Home::Time*, Home::People*>*, int h, int min, int sec);
		void add_people(Home::People* nwp);
		void add_relation(Home::People* nwp);
		vector<Home::People*> get_people(int);
		void setDirrection();
		void set_road_time();
		Lift(int m_, int n_, Logger* lg_, Compare<Home::Time*>* cmp, int num);
		~Lift();
	};

	vector<Lift*> lfts;
	AHeap<Time*, People*>* allpeople = nullptr;

	vector<bool> lvlState;
	vector<AHeap<Time*, People*>*> peopleOnLvl;

	void init(string setfile);
	void add_task(string filename);
	void start();
	void try_add_people(int h_, int min_);
	void lvl_step(int h_, int min_, int sec_);
	void lift_step(int h_, int min_, int sec_);
	Lift* find_near_lift(int num);
	~Home();
};

