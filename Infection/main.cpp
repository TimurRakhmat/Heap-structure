#include <iostream>
#include <fstream>
#include <string>
#include "generator.h"
#include "Infection.h"

using namespace std;

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));

	Generator gr(10000, 20, 0.1, 0.9, true);
	
	Infection inf("inf.txt");
	inf.start();
}