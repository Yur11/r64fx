#ifndef R64FX_MACHINE_GLOBAL_CONTEXT_HPP
#define R64FX_MACHINE_GLOBAL_CONTEXT_HPP

namespace r64fx{
    
class SoundDriver;
class SignalGraph;
    
struct MachineGlobalContext{
    SoundDriver* sound_driver = nullptr;
    SignalGraph* signal_graph = nullptr;
            
    long process();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_GLOBAL_CONTEXT_HPP