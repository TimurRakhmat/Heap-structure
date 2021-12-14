#include "Home.h"

void Home::init(string setfile)
{
	lt_lg = new Logger;
	pl_lg = new Logger;
	string lt_name = "lift.log", pl_name = "people.log";
	lt_lg->SetNext(lt_name, 2);
	pl_lg->SetNext(pl_name, 2);

	ifstream setF(setfile);
	setF >> n >> k;
	int m = 0;
	cmp = new TimeCmp;

	for (int i = 0; i < k; i++)
	{
		setF >> m;
		lfts.push_back(new Lift(m, n, lt_lg, cmp, i));
	}

	allpeople = new FibonacciHeap<Time*, People*>(cmp);
	for (int i = 0; i < n; i++)
	{
		lvlState.push_back(false);
		peopleOnLvl.push_back(new FibonacciHeap<Time*, People*>(cmp));
	}

	setF.close();
}

void Home::add_task(string filename)
{
	ifstream setF(filename);
	while (setF)
	{
		Time* tmp;
		People* pl;
		string time;
		int id, weight, start, end;
		setF >> time >> id >> weight >> start >> end;
		if (setF)
		{
			tmp = new Time(time);
			pl = new People(id, weight, start - 1, end - 1, tmp, pl_lg);
			allpeople->insert(tmp, pl);
		}
	}
}

void Home::start()
{
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			try_add_people(i, j);
			
			for (int kk = 0; kk < 60; kk++)
			{
				lvl_step(i, j, kk);
				lift_step(i, j, kk);
			}
		}
	}
}

void Home::try_add_people(int h_, int min_)
{
	if (allpeople->isEmpty())
		return;
	while (allpeople->findMin()->tm->get_int_time() == h_ * 60 + min_)
	{
		People* tpl = allpeople->findMin();
		peopleOnLvl[tpl->start]->insert(tpl->tm, tpl);
		allpeople->removeMin();
		if (allpeople->isEmpty())
			return;
	}
}

void Home::lvl_step(int h_, int min_, int sec_)
{
	for (int i = 0; i < n; i++)
	{
		if (!peopleOnLvl[i]->isEmpty())
		{
			lvlState[i] = true;
		}
	}
}

void Home::lift_step(int h_, int min_, int sec_)
{
	for (int i = 0; i < n; i++)
	{
		if (lvlState[i])
		{
			find_near_lift(i)->state_lvl[i] = true;
		}
	}

	for (int i = 0; i < k; i++)
	{
		if ((lvlState[lfts[i]->state] or lfts[i]->state_lvl[lfts[i]->state]) and lfts[i]->road_time == 0)
		{
			lvlState[lfts[i]->state] = false;
			lfts[i]->state_lvl[lfts[i]->state] = false;
			auto people_to_exit = lfts[i]->get_people(lfts[i]->state);
			for (auto it = people_to_exit.begin(); it != people_to_exit.end(); it++)
			{
				Time* etime = new Time(h_, min_, sec_);
				(*it)->tend = etime;
				delete (*it);
			}
			lfts[i]->try_to_add_people(peopleOnLvl[lfts[i]->state], h_, min_, sec_);
		}
		if (lfts[i]->road_time == 0)
			lfts[i]->setDirrection();
		lfts[i]->step();
	}
}

Home::Lift* Home::find_near_lift(int num)
{
	Lift* tmp = nullptr;
	int len = n + 1;
	for (int i = 0; i < k; i++)
	{
		if (lfts[i]->state == num)
		{
			return lfts[i];
		}
		if (num - lfts[i]->state > 0 and lfts[i]->dir >= 0)
		{
			if (abs(len) < abs(num - lfts[i]->state))
			{
				len = num - lfts[i]->state;
				tmp = lfts[i];
			}
		}
		else if (num - lfts[i]->state < 0 and lfts[i]->dir < 0)
		{
			if (abs(len) < abs(num - lfts[i]->state))
			{
				len = num - lfts[i]->state;
				tmp = lfts[i];
			}
		}
	}

	if (tmp)
		return tmp;
	len = n + 1;
	for (int i = 0; i < k; i++)
	{
		if (abs(len) > abs(num - lfts[i]->state))
		{
			len = num - lfts[i]->state;
			tmp = lfts[i];
		}
	}
	return tmp;
}

Home::~Home()
{
	for (int i = 0; i < k; i++)
	{
		delete lfts[i];
	}
	delete lt_lg;
	delete pl_lg;
	delete cmp;
}

string Home::People::get_id_string()
{
	if (neighbours.empty())
		return "None";
	string ans = "";
	while (!neighbours.empty())
	{
		ans += to_string(*neighbours.begin()) + ", ";
		neighbours.erase(neighbours.begin());
	}
	return ans;
}

Home::People::People(int id_, int weight_, int start_, int end_, Time* tm_, Logger* lg_) :\
id(id_), weight(weight_), start(start_), end(end_), tm(tm_), lg(lg_) {}

Home::People::~People()
{
	if (tm)
	{
		lg->Handle("arrived to state: " + to_string(start + 1), 2, tm->get_string());
		lg->Handle("my ID: " + to_string(id), 2, tm->get_string());
	}
	if (tstart)
	{
		lg->Handle("visit lift", 2, tstart->get_string());
		if (tend)
		{
			lg->Handle("lost lift on state: " + to_string(end + 1), 2, tend->get_string());
			lg->Handle("time in lift in sec: " + to_string(tend->get_int_time_sec() - tstart->get_int_time_sec()), 2, tend->get_string());
			lg->Handle("meet some people: " + get_id_string(), 2, tend->get_string());
			lg->Handle("count of overflow: " + to_string(oferflow_count), 2, tend->get_string());
		}
	}
	if (tm)
		delete tm;
	if (tstart)
		delete tstart;
	if (tend)
		delete tend;
}

string Home::Time::get_string()
{
	string ans = "0" + to_string(h), min_text = "0" + to_string(min), sec_text = "0" + to_string(sec);
	if (ans.length() > 2)
		ans = ans.substr(1);
	if (min_text.length() > 2)
		min_text = min_text.substr(1);
	if (sec_text.length() > 2)
		sec_text = sec_text.substr(1);
	return ans + ":" + min_text + ":" + sec_text;
}

void Home::Time::set_string(string time_s)
{
	int sep_pos = time_s.find(':');
	h = atoi(time_s.substr(0, sep_pos).c_str());
	min = atoi(time_s.substr(sep_pos + 1).c_str());
	sec = 0;
}

int Home::Time::get_int_time()
{
	return h * 60 + min;
}

int Home::Time::get_int_time_sec()
{
	return (h * 60 + min) * 60 + sec;
}

Home::Time::Time(string time_s)
{
	set_string(time_s);
}

Home::Time::Time(int h_, int min_, int sec_) : h(h_), min(min_), sec(sec_) {}

void Home::Lift::step()
{
	if (dir == 0)
	{
		rest_time++;
	}
	else
		work_time++;

	if (road_time)
	{
		road_time--;
		if (road_time == 0)
		{
			if (dir == 1)
			{
				state++;
			}
			
			if (dir == -1)
			{
				state--;
			}

			lvl_count++;
		}
	}
}

void Home::Lift::try_to_add_people(AHeap<Home::Time*, Home::People*>* heap, int h, int min, int sec)
{
	AHeap<Home::Time*, Home::People*>* tmp = new FibonacciHeap<Home::Time*, Home::People*>(tcmp);
	while (!heap->isEmpty())
	{
		Home::People* tmpP = heap->findMin();
		heap->removeMin();

		if (current_m + tmpP->weight <= m)
		{
			Time* startT = new Time(h, min, sec);
			tmpP->tstart = startT;
			add_people(tmpP);
		}
		else
		{
			overflow_count++;
			tmpP->oferflow_count++;
			add_relation(tmpP); // is need there?
			tmp->insert(tmpP->tm, tmpP);
		}
	}

	if (current_m > max_weight)
		max_weight = current_m;
	tmp->merge(heap);
}

void Home::Lift::add_people(Home::People* nwp)
{
	add_relation(nwp);
	peopls[nwp->end].push_back(nwp);
	state_lvl[nwp->end] = true;
	current_m += nwp->weight;
	total_weight += nwp->weight;
}

void Home::Lift::add_relation(Home::People* nwp)
{
	for (int i = 0; i < home_lvl; i++)
	{
		auto it = peopls[i].begin();
		for (it; it < peopls[i].end(); it++)
		{
			nwp->neighbours.insert((*it)->id);
			(*it)->neighbours.insert(nwp->id);
		}
	}
}

vector<Home::People*> Home::Lift::get_people(int index)
{
	auto tmp = peopls[index];
	for (auto it = tmp.begin(); it < tmp.end(); it++)
	{
		current_m -= (*it)->weight;
	}
	peopls[index].clear();
	return tmp;
}

void Home::Lift::setDirrection()
{
	bool save_state = false;
	if (dir == 1)
	{
		for (int i = state + 1; i < home_lvl; i++)
		{
			if (state_lvl[i])
			{
				save_state = true;
				break;
			}
		}
		if (save_state)
		{
			set_road_time();
			return;
		}
	}
	if (dir == -1)
	{
		for (int i = 0; i < state; i++)
		{
			if (state_lvl[i])
			{
				save_state = true;
				break;
			}
		}
		if (save_state)
		{
			set_road_time();
			return;
		}
	}

	if (dir == 0 or !save_state)
	{
		for (int i = 0; i < state; i++)
		{
			if (state_lvl[i])
			{
				save_state = true;
				break;
			}
		}
		if (save_state)
		{
			set_road_time();
			dir = -1;
			return;
		}
		for (int i = state + 1; i < home_lvl; i++)
		{
			if (state_lvl[i])
			{
				save_state = true;
				break;
			}
		}
		if (save_state)
		{
			set_road_time();
			dir = 1;
			return;
		}
	}
	dir = 0;
	road_time = 0;
}

void Home::Lift::set_road_time()
{
	road_time = 3 + (int)ceil(5 * (double)current_m / (double)m);
}

Home::Lift::Lift(int m_, int n_, Logger* lg_, Compare<Home::Time*>* cmp, int num) : m(m_), home_lvl(n_), lg(lg_), tcmp(cmp), number(num)
{
	for (int i = 0; i < n_; i++)
	{
		state_lvl.push_back(false);
		vector<People*> tmp;
		peopls.push_back(tmp);
	}
}

Home::Lift::~Lift()
{
	lg->Handle("start work", 2, "00:00:00");
	lg->Handle("my NUMBER: " + to_string(number + 1), 2, "00:00:00");
	lg->Handle("end work", 2, "24:00:00");
	lg->Handle("rest time: " + to_string(rest_time), 2, "24:00:00");
	lg->Handle("work time: " + to_string(work_time), 2, "24:00:00");
	lg->Handle("floors passed : " + to_string(lvl_count), 2, "24:00:00");
	lg->Handle("total weight: " + to_string(total_weight), 2, "24:00:00");
	lg->Handle("max weight: " + to_string(max_weight), 2, "24:00:00");
	lg->Handle("count of overflow : " + to_string(overflow_count), 2, "24:00:00");
}
