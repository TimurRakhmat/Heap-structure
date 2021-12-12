#include "Bank.h"

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	Bank bnk;
	bnk.init();
	bnk.start();
}