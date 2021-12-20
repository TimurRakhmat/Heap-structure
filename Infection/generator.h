#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <cstdlib> // для функций rand() и srand()
#include <ctime> 

using namespace std;

template <class T>
struct vertex {
public:
    vector<vertex*> adj;
    T name;
    vertex(T s) : name(s) {}
};


template<class T>
class graph
{
public:
    typedef map<T, vertex<T>*> vmap;
    vmap work;
    void addvertex(T name)
    {
        auto itr = work.find(name);
        if (itr == work.end())
        {
            vertex<T>* v;
            v = new vertex<T>(name);
            work[name] = v;
            return;
        }
    }
    void addedge(T from, T to)
    {
        vertex<T>* f = (work.find(from)->second);
        vertex<T>* t = (work.find(to)->second);
        if (find(f->adj.begin(), f->adj.end(), t) == f->adj.end())
            f->adj.push_back(t);
    }
    ~graph()
    {
        auto it = work.begin();
        for (it; it != work.end(); it++)
            delete it->second;
    }
};

class Generator
{
public:
	Generator(int p_count = 200, int day = 50, double percent_1 = 0.3, double percent_2 = 0.4, bool repeat = false);
};

