#include "MainClass.hpp"
#include <cmath>
#include <ctime>

int main(int argc, char** argv)
{
    srand(time(NULL));

    MainClass main(argc, argv);

    return main();
}