#ifndef R64FX_MACHINE_CONNECTION_IMPL_HPP
#define R64FX_MACHINE_CONNECTION_IMPL_HPP

#include "Array.hpp"

namespace r64fx{
    
class SignalConnection;
    
class MachineConnectionImpl : public Array<SignalConnection*>{
public:
    using Array::Array;
};    
    
}//namespace r64fx

#endif//R64FX_MACHINE_CONNECTION_IMPL_HPP