#ifndef R64FX_MACHINE_PORT_IMPL_HPP
#define R64FX_MACHINE_PORT_IMPL_HPP

#include "Array.hpp"

namespace r64fx{
        
class SignalSource;
    
struct MachineSourceImpl{
    Array<SignalSource*> sources;     
};


class SignalSink;

struct MachineSinkImpl{
    Array<SignalSink*> sinks;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_PORT_IMPL_HPP