#ifndef R64FX_MACHINE_POOL_CONTEXT_HPP
#define R64FX_MACHINE_POOL_CONTEXT_HPP

#include "LinkedList.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"

namespace r64fx{
    
struct MachinePoolContext{
    SoundDriver* sound_driver = nullptr;
    SignalGraph* signal_graph = nullptr;

    SoundDriverIOStatusPort* status_port = nullptr;
    
    MachinePoolContext();
    
    ~MachinePoolContext();
    
    long process();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_CONTEXT_HPP