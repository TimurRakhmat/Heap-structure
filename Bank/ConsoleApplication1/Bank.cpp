#include "Bank.h"

string make_time(int h, int min)
{
	string ans = to_string(h), min_text = "0" + to_string(min);
	if (min_text.length() > 2)
		min_text = min_text.substr(1);
	return ans + ":" + min_text;
}

void Bank::init()
{
	cmp = new MSGCmp;
	for (int i = 0; i < n; i++)
		deps.push_back(new Departament(i, cmp, pers[i]));
}

Bank::Departament* Bank::find_free()
{
	Departament* min = nullptr;
	double min_cap = 0;
	for (int i = 0; i < n; i++)
	{
		if (deps[i]->isFree()) {
			min_cap = deps[i]->getCapacity();
			min = deps[i];
			break;
		}
	}

	for (int i = 0; i < n; i++)
	{
		if (deps[i]->isFree() and deps[i]->getCapacity() < min_cap) {
			min_cap = deps[i]->getCapacity();
			min = deps[i];
		}
	}
	return min;
}

void Bank::addMSG(MSG* msg)
{
	if (deps[msg->a]->isFree())
	{
		deps[msg->a]->add_msg(msg);
		return;
	}
	find_free()->add_msg(msg);

}

void Bank::start()
{
	for (int i = 8; i < 22; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			auto v = generator.generate_msg(i, j, n);
			for (auto it = v.begin(); it != v.end(); it++)
			{
				addMSG(*it);
			}
			int stop_dep = (rand() % 100) + 1;
			if (current_l < lmax)
			{
				if (stop_dep <= l)
				{
					while (1)
					{
						int dep_index = rand() % n;
						if (!deps[dep_index]->isFree())
							continue;
						deps[dep_index]->setPause(find_free()->num);
						deps[dep_index]->msg_heap->merge(find_free()->msg_heap);
						current_l++;
						break;
					}
				}
			}
			for (int i = 0; i < n; i++)
				deps[i]->nextStep();
		}
	}
}

Bank::~Bank()
{
	for (int i = 0; i < n; i++)
		delete deps[i];


	delete cmp;
}

vector<Bank::MSG*> Bank::GeneratorMSG::generate_msg(int h, int min, int count)
{
	int msg_count = rand() % 10;
	if (h > 17)
		msg_count *= 0.8;
	msg_count -= 8;
	if (msg_count > 0)
	{
		int rn = rand() % 100;
		if (rn > 90)
			msg_count += rn % 10;
	}
	vector<Bank::MSG*> v;

	for (int i = 0; i < msg_count; i++)
	{
		int u = rand() % 10;
		int c = rand() % 6;
		string text = first[rand() % (first.size() - 1)] + " " + second[rand() % (second.size() - 1)] \
			+ " " + to_string((rand() % 1000) - 200)+ " " + third[rand() % (third.size() - 1)];
		int a = rand() % count;
		MSG* tmp = new MSG(u, h, min, a, c, text);
		
		v.push_back(tmp);
		
	}

	return v;
}

void Bank::Departament::add_msg(Bank::MSG* msg)
{
	msg_heap->insert(msg, msg);
	string text = "get msg to dep¹ " + to_string(msg->a + 1);
	lg.Handle(text, 0, make_time(h, min));
	msg_count++;
}

void Bank::Departament::nextStep()
{
	if (h < 12)
	{
		int chance = rand() % ((240 - h * 60 - min) / (personal_count - personal_current_count));
		if (chance < 2)
		{
			int that_count = rand() % (personal_count - personal_current_count);
			for (int i = 0; i < that_count; i++)
			{
				int id = num*1000 + personal_current_count;
				ps.push_back(new Slave(&lg, h, min, id));
				personal_current_count++;
			}
		}
	}
	if (h == 12 and min == 0)
	{
		int that_count = personal_count - personal_current_count;
		for (int i = 0; i < that_count; i++)
		{
			int id = num * 1000 + personal_current_count;
			ps.push_back(new Slave(&lg, h, min, id));
			personal_current_count++;
		}
	}

	for (int i = 0; i < personal_current_count; i++)
	{
		if (msg_heap->isEmpty())
			break;
		if (ps[i]->isFree())
		{
			ps[i]->setTask(msg_heap->findMin(), h, min);
			msg_heap->removeMin();
			msg_count--;
		}
	}

	for (int i = 0; i < personal_current_count; i++)
	{
		if (!ps[i]->finished_work)
			ps[i]->step(h, min);
	}

	if (!free)
	{
		if (extra_time == 0)
		{
			free = true;
			string text;
			text = to_string(num) + "  end of overload";
			lg.Handle(text, 4, make_time(h, min));
		}
		else
			extra_time--;
	}

	min++;
	if (min == 60)
	{
		h++;
		min = 0;
	}
}

double Bank::Departament::getCapacity()
{
	double prs_count = 0;
	for (int i = 0; i < personal_current_count; i++)
	{
		if (ps[i]->isFree())
			prs_count += 1;
	}
	if (prs_count == 0)
		return 2*msg_count;
	return (double)msg_count/(double)personal_current_count;
}

Bank::Departament::Departament(int num_, Compare<Bank::MSG*>* cmp, int n) : num(num_ + 1), personal_count(n)
{
	msg_heap = new BinomialHeap<MSG*, MSG*>(cmp);
	string name1 = "full_log_dep_" + to_string(num) + ".log", name2 = "log_dep_" + to_string(num) + ".log",\
		msg_file = "msg_log_dep_" + to_string(num) + ".log";
	lg.SetNext(name1, 1)->SetNext(name2, 3)->SetNext(msg_file, 0);
	string text = "Departament ¹" + to_string(num) + " started work;";
	lg.Handle(text, 3, "8:00");
}

Bank::Departament::~Departament()
{
	string text = "Departament ¹" + to_string(num) + " finished work;";
	lg.Handle(text, 3, "21:00");
	for (int i = 0; i < personal_count; i++)
		delete ps[i];
	while (!msg_heap->isEmpty())
	{
		delete msg_heap->findMin();
		msg_heap->removeMin();
	}
}

void Bank::Departament::setPause(int num_of_dep)
{
	free = false;
	extra_time = rand() % 120 + 60;
	string text;
	text = to_string(num) + "  is overload, send msg to departament ¹" + to_string(num_of_dep + 1);
	lg.Handle(text, 4, make_time(h, min));
}

Bank::Departament::Slave::Slave(Logger* lg_, int h, int min, int id_)
{
	id = id_;
	lg_s = lg_;
	string text;
	text = to_string(id) + " arrived to work;";
	lg_s->Handle(text, 2, make_time(h, min));
}

void Bank::Departament::Slave::setTask(MSG* msg, int h, int min)
{
	free = false;
	task_time = Bank::t + (rand() % (Bank::td * 2 + 1)) - Bank::td + msg->c * 2;
	string text;
	text = to_string(id) + " started process: " + msg->text + "; time to task- " + to_string(task_time);
	lg_s->Handle(text, 2, make_time(h, min));
	try_dinner(h, min);
}

void Bank::Departament::Slave::try_dinner(int h, int min)
{
	if (have_dinner)
		return;
	int chance = 100;
	if (work_time < 240)
		chance = rand() % 20;
	else
	{
		chance = rand() % 4;
	}
	if (chance < 3)
	{
		dinner_time = 60;
		dinner = true;
		free = false;
		string text;
		text = to_string(id) + " started dinner;";
		lg_s->Handle(text, 1, make_time(h, min));
	}
}

void Bank::Departament::Slave::stop_dinner(int h, int min)
{
	dinner = false;
	free = true;
	string text;
	text = to_string(id) + " finished dinner;";
	lg_s->Handle(text, 1, make_time(h, min));
	have_dinner = true;
}

void Bank::Departament::Slave::step(int h, int min)
{
	work_time++;
	if (task_time > 0)
	{
		task_time--;
		if (task_time == 0)
		{
			string text;
			text = to_string(id) + " finished process;";
			lg_s->Handle(text, 2, make_time(h, min));
			free = true;
			try_dinner(h, min);
		}
	}
	if (dinner)
	{
		dinner_time--;
		if (dinner_time == 0)
		{
			stop_dinner(h, min);
		}
	}
	if (work_time >= 9 * 60)
	{
		string text;
		text = to_string(id) + " left work;";
		lg_s->Handle(text, 2, make_time(h, min));
		finished_work = true;
		free = false;
	}
}
