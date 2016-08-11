#include "MachinePoolContext.hpp"

namespace r64fx{
    
    
MachinePoolContext::MachinePoolContext()
{
    sound_driver = SoundDriver::newInstance();
    status_port = sound_driver->newStatusPort();
    signal_graph = new SignalGraph;
}
    
    
MachinePoolContext::~MachinePoolContext()
{
    if(signal_graph)
    {
        delete signal_graph;
    }
    
    if(sound_driver)
    {
        SoundDriver::deleteInstance(sound_driver);
    }
}
    
        
long MachinePoolContext::process()
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
    
    return 0;
}
    
}//namespace r64fx