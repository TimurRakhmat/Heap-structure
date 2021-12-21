#include "generator.h"


Generator::Generator(int p_count, int day, double percent_1, double percent_2, bool repeat)
{
	ifstream name("name.txt");
	ifstream sname("surname.txt");

	vector<string>  namev(100);
	vector<string> snamev(100);

	for (int i = 0; i < 100; i++)
	{
		name >> namev[i];
		sname >> snamev[i];
	}

	name.close();
	sname.close();
	graph<int> gr;

	for (int i = 0; i < p_count; i++)
	{
		gr.addvertex(i);
	}

	double p = 14;

	for (int i = 0; i < p_count; i++)
	{

		gr.addedge(i, (i + 1) % p_count); // make adjacent graph
		gr.addedge((i + 1) % p_count, i);

		int c = 4 + rand() % 6;
		for (int j = 0; j < c; j++) // add some edge
		{
			int index = rand() % p_count;
			if (index != i)
			{
				gr.addedge(i, index);
				gr.addedge(index, i);
			}
		}
	}

	ofstream file("inf.txt");
	for (int i = 0; i < p_count; i++)
	{
		file << i << "," << namev[rand() % 100] << "," <<  snamev[rand() % 100];
		for (auto it = gr.work[i]->adj.begin(); it != gr.work[i]->adj.end(); it++)
		{
			file << "," << (*it)->name;
		}
		file << endl;
	}

	file << percent_1 << " " << percent_2 << " " << day << " " << boolalpha << repeat;

	file.close();
}