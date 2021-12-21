#include <iostream>
#include <fstream>
#include <string>
#include "generator.h"
#include "Infection.h"

using namespace std;

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));

	Generator gr(250, 6, 0.1, 0.95, true);
	
	Infection inf("inf.txt");
	inf.start();
}