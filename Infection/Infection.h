#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "generator.h"
#include <vector>
#include "../Logger.h"

using namespace std;

class Infection
{
	class Man
	{
	public:
		int id;
		bool isSick = false;
		bool wasSick = false;
		int count_of_sick = 0;
		int count_of_resick = 0;
		string name, surname;
		vector<int> neibs;
		void setSick();
		void getWell();
	};

	double p1 = 1, p2 = 0;
	int days = 1, count = 0;
	bool repeat = false;

	vector<Man*> ppls;
	

	void parseline(string& line);
	void setSettings(string& line);
public:
	Infection(string fname);
	void start();
	~Infection();
};
