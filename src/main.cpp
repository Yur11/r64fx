#include "Timer.hpp"
#include "sleep.hpp"
#include "MachineManager.hpp"

using namespace r64fx;

int main(int argc, char* argv[])
{
    MachineManager mm;
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
