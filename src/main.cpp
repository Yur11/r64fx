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
    sdm.createAudioInput("in");
    sdm.createAudioOutput("out");
    sdm.createMidiInput("midi_in");
    sdm.createMidiOutput("midi_out");
    sleep_microseconds(500000);
    sdm.routeThrough("out", "in");
    
    Timer timer;
    timer.onTimeout([](Timer* timer, void* arg){
        static bool on = true;
        
        auto sdm = (SoundDriverMachine*) arg;
        if(on)
        {
            sdm->sendMidiMessage(MidiMessage::NoteOn(1, 69, 127));
            on = false;
        }
        else
        {
            sdm->sendMidiMessage(MidiMessage::NoteOff(1, 69, 127));
            on = true;
        }
    }, &sdm);
    timer.setInterval(1000000);
//     timer.start();
    
    while(true)
    {
        Timer::runTimers();
        sleep_microseconds(500);
    }

    return 0;
}
