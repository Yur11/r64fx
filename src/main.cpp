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
    sdm.createAudioInput("in");
    sdm.createAudioOutput("out");
    sdm.createMidiInput("midi_in");
    sdm.createMidiOutput("midi_out");

    RouterMachine rm(&mp);
    rm.setName("Router");
    
    for(auto machine : mp)
    {
        cout << machine->name() << "\n";
    }
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
