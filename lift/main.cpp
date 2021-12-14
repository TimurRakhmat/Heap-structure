#include <iostream>
#include "Home.h"

using namespace std;

int main(int argc, char* argv[])
{
    Home hm;
    hm.init(argv[1]);
    for (int i = 2; i < argc; i++)
        hm.add_task(argv[i]);
    hm.start();
}