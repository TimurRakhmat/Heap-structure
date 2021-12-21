#include "Hospitale.h"

Hospitale::Patient::Patient(int id_, string name_, int tstart_, bool issick_):id(id_), name(name_), tstart(tstart_), isSick(issick_)
{

}

Hospitale::Doctor::Doctor(int id_):id(id_)
{

}

void Hospitale::add_patient(int i)
{
	auto v = generator->generate(i);
	for (auto it = v.begin(); it != v.end(); it++)
	{
		string text;
		text = "patient name: " + (*it)->name + ", his id:" + to_string((*it)->id) + " came to hospitale";
		if ((*it)->isSick)
			text += "he is sick";
		lg->Handle(text, 1, time_to_str(i));
		if ((*it)->isSick)
		{
			sickqueue->insert(i, *it);
			ssize++;
		}
		else
		{
			hlhqueue->insert(i, *it);
			hsize++;
		}
	}
}

void Hospitale::try_to_enter(int time)
{
	if (ssize == 0 and hsize == 0)
		return;
	for (int i = rsize; i < N; i++) {
		Patient* tmp = nullptr;
		if (rsize > 0)
		{
			if (roomqueue->findMin()->isSick)
			{
				if (ssize > 0)
				{
					tmp = sickqueue->findMin();
					sickqueue->removeMin();
					ssize--;
					roomqueue->insert(tmp->tstart, tmp);
					rsize++;
				}
				else
					return; //empty queue
			}
			else
			{
				if (hsize > 0)
				{
					tmp = hlhqueue->findMin();
					hlhqueue->removeMin();
					hsize--;
					roomqueue->insert(tmp->tstart, tmp);
					rsize++;
				}
				else
					return; //empty queue
			}
		}
		else
		{
			if (hsize > 0 and ssize > 0)
			{
				if (hlhqueue->findMin()->tstart < sickqueue->findMin()->tstart)
				{
					tmp = hlhqueue->findMin();
					hlhqueue->removeMin();
					hsize--;
					roomqueue->insert(tmp->tstart, tmp);
					rsize++;
				}
				else
				{
					tmp = sickqueue->findMin();
					sickqueue->removeMin();
					ssize--;
					roomqueue->insert(tmp->tstart, tmp);
					rsize++;
				}
			}
			else if (hsize > 0)
			{
				tmp = hlhqueue->findMin();
				hlhqueue->removeMin();
				hsize--;
				roomqueue->insert(tmp->tstart, tmp);
				rsize++;
			}
			else
			{
				tmp = sickqueue->findMin();
				sickqueue->removeMin();
				ssize--;
				roomqueue->insert(tmp->tstart, tmp);
				rsize++;
			}
		}
	}
}

void Hospitale::try_to_infect(int time)
{
	if (hsize == 0 or ssize == 0)
		return;

	if (rand() % 100 + 1 > P)
		return;

	string text = "Queue was infected: " + to_string(hsize) + "peoples was infected";
	lg->Handle(text, 5, time_to_str(time));
	
	// we could merge, but need to set all people flag isSick

	while (hsize > 0)
	{
		auto tmp = hlhqueue->findMin();
		tmp->isSick = true;
		hlhqueue->removeMin();
		hsize--;
		ssize++;
		sickqueue->insert(tmp->tstart, tmp);
	}
}

void Hospitale::doc_steps(int time)
{
	for (int i = 0; i < M; i++)
	{
		if (doc_vec[i]->isBusy)
		{
			doc_vec[i]->current_time_for_pat--;
			if (doc_vec[i]->current_time_for_pat == 0)
			{
				if (doc_vec[i]->onAdvice)
				{
					end_advice(doc_vec[i], time);
				}
				else
				{
					make_log_patient(doc_vec[i], time);
				}
				doc_heap->insert(doc_vec[i]->wtime, doc_vec[i]);
			}
		}

		if (doc_vec[i]->isBusy and !doc_vec[i]->onAdvice)
		{
			is_need_help(doc_vec[i], time);
		}

	}
}

void Hospitale::end_advice(Doctor* doc, int time)
{
	doc->wtime += doc->time_for_pat;
	doc->time_for_pat = 0;
	doc->isBusy = false;
	doc->onAdvice = false;

	string text;
	text = "doc id: " + to_string(doc->id) + " finished help doc id: " + to_string(doc->help_doc_id);
	lg->Handle(text, 4, time_to_str(time));
}

void Hospitale::make_log_patient(Doctor* doc, int time)
{
	string text;
	text = "doc id: " + to_string(doc->id) + ", patient id: " + to_string(doc->my_pat->id) + " left doctor, visit lasted:" + to_string(doc->time_for_pat);
	lg->Handle(text, 2, time_to_str(time));

	doc->wtime += doc->time_for_pat;
	doc->time_for_pat = 0;
	doc->isBusy = false;
	doc->was_Advice = false;

	text = "patient name: " + doc->my_pat->name + ", his id:" + to_string(doc->my_pat->id) + " left hospitale, was there: " + to_string(time - doc->my_pat->tstart);
	lg->Handle(text, 1, time_to_str(time));

	delete doc->my_pat;
	doc->my_pat = nullptr;
}

void Hospitale::is_need_help(Doctor* doc, int time)
{
	if (doc_heap->isEmpty())
		return;

	if (doc->was_Advice)
		return;

	if (rand() % 100 + 1 > P2)
		return;

	int extra_t = rand() % (T + 1) + 1;
	auto tmpd = doc_heap->findMin();
	doc_heap->removeMin();

	doc->current_time_for_pat += extra_t;
	doc->time_for_pat += extra_t;
	doc->was_Advice = true;

	tmpd->onAdvice = true;
	tmpd->isBusy = true;
	tmpd->current_time_for_pat = extra_t;
	tmpd->time_for_pat = extra_t;
	tmpd->help_doc_id = doc->id;

	string text;
	text = "doc id: " + to_string(tmpd->id) + " arrive to help doc id: " + to_string(doc->id);
	lg->Handle(text, 4, time_to_str(time));
}

void Hospitale::try_to_heal(int time)
{
	if (rsize < 0)
		return;
	if (doc_heap->isEmpty())
		return;

	while (!doc_heap->isEmpty() and rsize > 0)
	{
		auto tmp_doc = doc_heap->findMin();
		tmp_doc->my_pat = roomqueue->findMin();
		roomqueue->removeMin();
		rsize--;
		doc_heap->removeMin();
		tmp_doc->isBusy = true;
		tmp_doc->time_for_pat = rand() % (T + 1) + 1;
		tmp_doc->current_time_for_pat = tmp_doc->time_for_pat;

		string text;
		text = "doc id: " + to_string(tmp_doc->id) + ", patient id: " + to_string(tmp_doc->my_pat->id) + " came to doctor";
		lg->Handle(text, 2, time_to_str(time));
	}

}

bool Hospitale::try_exit(int)
{
	if (rsize > 0 or ssize > 0 or hsize > 0)
		return false;

	for (int i = 0; i < M; i++)
	{
		if (doc_vec[i]->isBusy)
			return false;
	}

	return true;
}

string time_to_str(int time)
{
	string ans = "0" + to_string(time / 60), min_text = "0" + to_string(time % 60);
	if (ans.length() > 2)
		ans = ans.substr(1);
	if (min_text.length() > 2)
		min_text = min_text.substr(1);
	return ans + ":" + min_text;
}

Hospitale::Hospitale(int n, int m, int t) : N(n), M(m), T(t)
{
	cmp = new IntCompare;
	hlhqueue = new BinomialHeap<int, Patient*>(cmp);
	sickqueue = new BinomialHeap<int, Patient*>(cmp);
	roomqueue = new BinomialHeap<int, Patient*>(cmp);
	doc_heap = new BinomialHeap<int, Doctor*>(cmp);

	generator = new Generator();

	LogBuilder lb;
	string name = "log/hos.log";
	lb.add_next(name, 0);
	lg = new Logger(&lb);

	string text;
	for (int i = 0; i < M; i++)
	{
		auto tmp = new Doctor(i);
		text = "doc id: " + to_string(tmp->id) + ", enter to Hospitale";
		lg->Handle(text, 3, "00:00");
		doc_heap->insert(0, tmp);
		doc_vec.push_back(tmp);
	}
}

void Hospitale::start()
{
	int i = 0; //step
	while (1) 
	{
		if(i < 12 * 60) // add new people in first 12 hours
		{
			add_patient(i);
		}

		try_to_enter(i);
		try_to_infect(i);

		doc_steps(i);
		try_to_heal(i);

		if (i > 12 * 60) // work until all patients have been examined
		{
			if (try_exit(i))
				break;
		}

		i++;
	}
	
	string text;
	for (int j = 0; j < M; j++)
	{
		text = "doc id: " + to_string(doc_vec[j]->id) + ", left Hospitale, he work today arround: " + time_to_str(doc_vec[j]->wtime);
		lg->Handle(text, 3, time_to_str(i));
		delete doc_vec[j];
	}
}

Hospitale::~Hospitale()
{
	delete doc_heap, hlhqueue, roomqueue, sickqueue;
	delete cmp;
	delete lg;
}

Hospitale::Generator::Generator()
{
	ifstream name("../Infection/name.txt");
	ifstream sname("../Infection/surname.txt");

	string tmp;

	for (int i = 0; i < 100; i++)
	{
		name >> tmp;
		namev.push_back(tmp);
		sname >> tmp;
		snamev.push_back(tmp);
	}

	name.close();
	sname.close();
}

vector<Hospitale::Patient*> Hospitale::Generator::generate(int time)
{
	vector<Hospitale::Patient*> v;
	int ppl_count = rand() % 100;
	if (ppl_count > 90)
	{
		ppl_count = 1;
		int rn = rand() % 100;
		if (rn > 90)
			ppl_count += rn % 10;
	}
	else
		return v;
	

	for (int i = 0; i < ppl_count; i++)
	{
		string name = namev[rand() % 100], sname = snamev[rand() % 100];
		bool sick = false;
		if (rand() % 100 > 70)
			sick = true;

		Patient* tmp = new Patient(id_cnt, name + " " + sname, time, sick);
		id_cnt++;
		v.push_back(tmp);
	}

	return v;
}
