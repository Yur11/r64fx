#include "PlayerController.hpp"
// #include "Widget_Dummy.hpp"
// #include "Timer.hpp"
// #include "sleep.hpp"
// #include <iostream>

using namespace std;
using namespace r64fx;

namespace r64fx{

bool g_running = true;

}//namespace r64fx


int main(int argc, char* argv[])
{
    PlayerController p;
    p.exec();

    return 0;
}
