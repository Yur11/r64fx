#include "Timer.hpp"
#include "sleep.hpp"
#include "MachinePool.hpp"
#include "DummyMachine.hpp"

using namespace r64fx;

int main(int argc, char* argv[])
{
    MachinePool mm;
    DummyMachine dm1(&mm);
    dm1.deploy();
    DummyMachine dm2(&mm);
    dm2.deploy();
    DummyMachine dm3(&mm);
    dm3.deploy();
    
    mm.withdrawAll();
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
