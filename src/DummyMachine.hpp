#ifndef R64FX_DUMMY_MACHINE_HPP
#define R64FX_DUMMY_MACHINE_HPP

#include "MachineIface.hpp"

namespace r64fx{

class DummyMachine : public MachineIface{
public:
    virtual void forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg) = 0;
    
private:
    virtual void deploymentEvent() = 0;
    
    virtual void withdrawalEvent() = 0;
    
    virtual void messageRecievedFromImpl(const MachineMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_DUMMY_MACHINE_HPP
