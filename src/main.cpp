#include "Timer.hpp"
#include "sleep.hpp"
#include "MachineManager.hpp"
#include "DummyMachine.hpp"

using namespace r64fx;

int main(int argc, char* argv[])
{
    MachineManager mm;
    DummyMachine dm(&mm);
    dm.deploy();
    dm.sayHello();
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
