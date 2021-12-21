#define _CRT_SECURE_NO_WARNINGS
#include "Infection.h"


Infection::Infection(string fname)
{
	ifstream file(fname);
	string line;
	while (file)
	{
		getline(file, line);
		if (line.find(',') != line.npos) {
			parseline(line);
			count++;
		}
		else
		{
			setSettings(line);
			file.close();
			break;
		}
	}
	file.close();
	ppls[rand() % count]->setSick();
}

void Infection::parseline(string& line)
{
	auto man = new Man;
	int pos = line.find(',');
	man->id = atoi(line.substr(0, pos).c_str());
	line = line.substr(pos + 1);
	pos = line.find(',');
	man->name = line.substr(0, pos);
	line = line.substr(pos + 1);
	pos = line.find(',');
	man->surname = line.substr(0, pos);
	line = line.substr(pos + 1);
	pos = 0;
	string num_bf = "";
	for (int i = 0; i < line.length(); i++)
	{
		if (line[i] != ',')
		{
			num_bf += line[i];
		}
		else
		{
			man->neibs.push_back(atoi(num_bf.c_str()));
			num_bf = "";
		}
	}
	ppls.push_back(man);
}

void Infection::setSettings(string& line)
{
	int pos = line.find(' ');
	p1 = atof(line.substr(0, pos).c_str()) * 100;
	line = line.substr(pos + 1);

	pos = line.find(' ');
	p2 = atof(line.substr(0, pos).c_str()) * 100;
	line = line.substr(pos + 1);

	pos = line.find(' ');
	days = atoi(line.substr(0, pos).c_str());
	line = line.substr(pos + 1);

	if (line == "true")
		repeat = true;
}

void Infection::start()
{
	for (int i = 0; i < days; i++)
	{
		// setSick

		for (int j = 0; j < count; j++)
		{
			if (ppls[j]->wasSick and !repeat)
				break;
			for (auto it = ppls[j]->neibs.begin(); it != ppls[j]->neibs.end(); it++)
			{
				if (ppls[*it]->isSick)
				{
					if (rand() % 100 + 1 < p1)
					{					
						ppls[j]->setSick();
						break;
					}
				}
			}
		}

		//getWell

		for (int j = 0; j < count; j++)
		{
			if (ppls[j]->isSick) {
				if (rand() % 100 + 1 < p2)
				{
					ppls[j]->getWell();
					break;
				}
			}
		}
	}
}

Infection::~Infection()
{
	LogBuilder lb;

	time_t curr_time;
	curr_time = time(NULL);

	tm* tm_local = localtime(&curr_time);
	string name = "log\\" + to_string(tm_local->tm_hour) + "_" + to_string(tm_local->tm_min) + "_" + to_string(tm_local->tm_sec) + ".log";
	lb.add_next(name, 0);
	Logger lg(&lb);

	lg.Handle("----------------was never sick------------------", 0, "notSick");
	for (int j = 0; j < count; j++)
	{
		if (!ppls[j]->isSick and !ppls[j]->wasSick)
		{
			lg.Handle(ppls[j]->name + " " + ppls[j]->surname + " - " + to_string(ppls[j]->id), 0, "notSick");
		}
	}
	lg.Handle("----------------was never sick------------------", 0, "notSick");

	lg.Handle("               ", 6, "-");

	lg.Handle("----------------getWell now------------------", 0, "getWell");
	for (int j = 0; j < count; j++)
	{
		if (!ppls[j]->isSick and ppls[j]->wasSick)
		{
			lg.Handle(ppls[j]->name + " " + ppls[j]->surname + " - " + to_string(ppls[j]->id), 1, "getWell");
		}
	}
	lg.Handle("----------------getWell now------------------", 0, "getWell");

	lg.Handle("               ", 6, "-");

	lg.Handle("----------------get well, not other------------------", 0, "getWell, but");
	for (int j = 0; j < count; j++)
	{
		bool flag = true;
		if (!ppls[j]->isSick and ppls[j]->wasSick)
		{
			for (auto it = ppls[j]->neibs.begin(); it != ppls[j]->neibs.end(); it++)
			{
				if (!ppls[*it]->isSick)
				{
					flag = false;
					break;
				}
			}

			if (flag)
			{
				lg.Handle(ppls[j]->name + " " + ppls[j]->surname + " - " + to_string(ppls[j]->id) , 2, "getWell, not other");
			}
		}
	}
	lg.Handle("----------------get well, not other------------------", 0, "getWell, but");

	lg.Handle("               ", 6, "-");

	lg.Handle("----------------were never sick, but around------------------", 0, "was not sick, but");
	for (int j = 0; j < count; j++)
	{
		bool flag = true;
		if (!ppls[j]->isSick and !ppls[j]->wasSick)
		{
			for (auto it = ppls[j]->neibs.begin(); it != ppls[j]->neibs.end(); it++)
			{
				if (!ppls[*it]->isSick and !ppls[*it]->wasSick)
				{
					flag = false;
					break;
				}
			}

			if (flag)
			{
				lg.Handle(ppls[j]->name + " " + ppls[j]->surname + " - " + to_string(ppls[j]->id), 3, "was not sick, but other");
			}
		}
	}
	lg.Handle("----------------were never sick, but around------------------", 0, "was not sick, but");

	

	
	if (repeat)
	{
		lg.Handle("               ", 6, "-");
		lg.Handle("----------------Were sick more one time------------------", 0, "sick more");
		for (int j = 0; j < count; j++)
		{
			if (ppls[j]->count_of_sick > 1)
			{
				lg.Handle(ppls[j]->name + " " + ppls[j]->surname + " - " + to_string(ppls[j]->id), 4, "sick more one");
			}
		}
		lg.Handle("----------------Were sick more one time------------------", 0, "sick more");
	}
	

	for (int j = 0; j < count; j++)
	{
		delete ppls[j];
	}
}

void Infection::Man::setSick()
{
	if (!isSick) {
		isSick = true;
		count_of_sick++;
	}
}

void Infection::Man::getWell()
{
	isSick = false;
	wasSick = true;
	count_of_resick++;
}
