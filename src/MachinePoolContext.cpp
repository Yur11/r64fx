#include "MachinePoolContext.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
        
long MachinePoolContext::process()
{
    static SoundDriverIOStatusPort* status_port = nullptr;
    if(sound_driver)
    {
        if(status_port)
        {
            SoundDriverIOStatus status;
            if(status_port->readStatus(&status, 1))
            {
                if(signal_graph)
                {
                    signal_graph->prepare();
                    for(int i=0; i<sound_driver->bufferSize(); i++)
                    {
                        signal_graph->processSample(i);
                    }
                    signal_graph->finish();
                }
            }
            else
            {
            }
        }
        else
        {
            status_port = sound_driver->newStatusPort();
        }
    }
    
    return 0;
}
    
}//namespace r64fx