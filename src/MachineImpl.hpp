#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"

namespace r64fx{
    
class MachineIface;
    
class MachineImpl{
    MachineIface* m_iface = nullptr;

public:
    MachineImpl(MachineIface* machine_iface)
    : m_iface(machine_iface)
    {
        
    }
    
    virtual ~MachineImpl() {}
    
    virtual void messageRecievedFromIface(const MachineMessage &msg) = 0;
    
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_IMPL_HPP
