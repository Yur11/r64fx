#include "Timer.hpp"
#include "sleep.hpp"
#include "MachinePool.hpp"
#include "DummyMachine.hpp"
#include "SoundDriverMachine.hpp"
#include <unistd.h>

using namespace r64fx;

int main(int argc, char* argv[])
{
    MachinePool mm;
    SoundDriverMachine sdm(&mm);
    sdm.deploy();
    sdm.enable();
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
