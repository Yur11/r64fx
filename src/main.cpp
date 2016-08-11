#include "Timer.hpp"
#include "sleep.hpp"
#include "MachinePool.hpp"
#include "SoundDriverMachine.hpp"
#include "RouterMachine.hpp"
#include "PlayerMachine.hpp"
#include "SoundFile.hpp"
#include <unistd.h>

#include <iostream>

using namespace std;
using namespace r64fx;

int main(int argc, char* argv[])
{
    SoundFile sf("/home/yurii/Sound/Samples/24940__vexst__the-winstons-amen-brother-full-solo-4-bars.wav", SoundFile::Mode::Read);
    if(!sf.isGood())
    {
        cerr << "File is bad!\n";
    }
    else
    {
        cout << "OK!\n";
        cout << sf.componentCount() << "\n";
        cout << sf.frameCount() << "\n";
        
        float* data = new float[sf.componentCount() * (sf.frameCount() + 1)];
        cout << sf.readFrames(data, sf.frameCount()) << "\n";
        cout << data << "\n";
        
        MachinePool mp;
    
        SoundDriverMachine sdm(&mp);
        sdm.setName("Sound Driver");
        sdm.deploy();
        sdm.enable();
        auto in = sdm.createAudioInput("in");
        auto out = sdm.createAudioOutput("out");
        sdm.createMidiInput("midi_in");
        sdm.createMidiOutput("midi_out");

        PlayerMachine pm(&mp);
        pm.deploy();
        pm.playData(data, sf.frameCount(), sf.componentCount());
        auto player_out = pm.signalSource();
        
        int i=0;
        while(!player_out->handle())
        {
            cout << i++ << "\n"; 
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        
        for(int i=0; i<10; i++)
        {
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        
        RouterMachine rm(&mp);
        rm.setName("Router");
        rm.deploy();
        rm.makeConnection(player_out, out);
        
        while(true)
        {
            Timer::runTimers();
            sleep_microseconds(500);
        }
    
        delete[] data;
    }

    return 0;
}
