#ifndef R64FX_MACHINE_POOL_CONTEXT_HPP
#define R64FX_MACHINE_POOL_CONTEXT_HPP

#include "LinkedList.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"

namespace r64fx{
    
struct MachinePoolContext{
    SoundDriver* sound_driver = nullptr;
    SoundDriverIOStatusPort* status_port = nullptr;
    
    SignalGraph* root_graph       = nullptr;
    SignalGraph* input_subgraph   = nullptr;
    SignalGraph* main_subgraph    = nullptr;
    SignalGraph* output_subgraph  = nullptr;
    
    MachinePoolContext();
    
    ~MachinePoolContext();
    
    long process();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_CONTEXT_HPP