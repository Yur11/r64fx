#include "Timer.hpp"
#include "sleep.hpp"
#include "MachinePool.hpp"
#include "SoundDriverMachine.hpp"
#include "RouterMachine.hpp"
#include <unistd.h>

#include <iostream>

using namespace std;
using namespace r64fx;

int main(int argc, char* argv[])
{
    MachinePool mp;
    
    SoundDriverMachine sdm(&mp);
    sdm.setName("Sound Driver");
    sdm.deploy();
    sdm.enable();
    auto in = sdm.createAudioInput("in");
    auto out = sdm.createAudioOutput("out");
    sdm.createMidiInput("midi_in");
    sdm.createMidiOutput("midi_out");

    for(int i=0; i<10; i++)
    {
        Timer::runTimers();
        sleep_microseconds(5000);
    }
    
    RouterMachine rm(&mp);
    rm.setName("Router");
    rm.deploy();
    auto c = rm.makeConnection(in, out);
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
