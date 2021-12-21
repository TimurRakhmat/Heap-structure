#pragma once
#include <cstdlib> // для функций rand() и srand()
#include <ctime> 
#include <string>
#include <vector>
#include <iostream>
#include "..\Logger.h"
#include"..\BinaryHeap.h"
#include"..\FibonacciHeap.h"
#include"..\BinomialHeap.h"
#include <queue>

using namespace std;

string time_to_str(int);

class Hospitale
{
protected:
	int N = 8; // place in hall
	int M = 3; // count of doc
	int T = 10; // max visit time

	int P = 5, P2 = 10; // p - infect queue, p2 - doc need help

	Logger* lg;

	class Patient
	{
	public:
		int id;
		string name;
		bool isSick;
		int tstart;
		int tend;
		Patient(int, string, int, bool);
	};

	class Doctor
	{
	public:
		bool isBusy = false, onAdvice = false;
		int help_doc_id;
		bool was_Advice = false;
		int time_for_pat = 0;
		int current_time_for_pat = 0;
		int id;
		int wtime = 0;
		Patient* my_pat = nullptr;
		Doctor(int id_);
	};

	class Generator
	{
	public:
		vector<string> namev;
		vector<string> snamev;
		int id_cnt = 1;
		Generator();
		vector<Patient*> generate(int time);
	};
	IntCompare* cmp = nullptr;
	Generator* generator;
	AHeap<int, Patient*>* hlhqueue = nullptr;
	int hsize = 0;
	AHeap<int, Patient*>* sickqueue = nullptr;
	int ssize = 0;
	AHeap<int, Patient*>* roomqueue = nullptr;
	int rsize = 0;

	AHeap<int, Doctor*>* doc_heap = nullptr;
	vector<Doctor*> doc_vec;

	void add_patient(int);
	void try_to_enter(int);
	void try_to_infect(int);
	void doc_steps(int);
	void make_log_patient(Doctor*, int);
	void is_need_help(Doctor*, int);
	void end_advice(Doctor*, int);
	void try_to_heal(int);
	bool try_exit(int);
public:
	Hospitale(int n = 8, int m = 3, int t = 10);
	void start();
	~Hospitale();
};

