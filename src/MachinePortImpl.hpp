#ifndef R64FX_MACHINE_PORT_IMPL_HPP
#define R64FX_MACHINE_PORT_IMPL_HPP

#include "Array.hpp"

namespace r64fx{
        
class SignalSource;
    
class MachineSourceImpl : public Array<SignalSource*>{
public:
    using Array::Array;
};


class SignalSink;

class MachineSinkImpl : public Array<SignalSink*>{
public:
    using Array::Array;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_PORT_IMPL_HPP