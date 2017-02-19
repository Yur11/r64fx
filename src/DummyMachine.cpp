#include "DummyMachine.hpp"
#include "MachineImpl.hpp"
#include "MachinePoolThread.hpp"

namespace r64fx{

struct DummyMachineImpl : public MachineImpl{
    DummyMachineImpl(MachineIface* machine_iface, MachinePoolThreadImpl* thread)
    : MachineImpl(machine_iface, thread)
    {

    }

    virtual void messageRecievedFromIface(const MachineMessage &msg)
    {

    }
};


void DummyMachine::forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg)
{

}


void DummyMachine::deploymentEvent()
{
    deploymentThread()->deployImpl([](MachineIface* iface,  MachinePoolThreadImpl* thread) -> MachineImpl* {
        return new DummyMachineImpl(iface, thread);
    }, this);
}


void DummyMachine::withdrawalEvent()
{
    deploymentThread()->withdrawImpl([](MachineImpl* impl,  MachinePoolThreadImpl* thread){
        auto dummy_impl = static_cast<DummyMachineImpl*>(impl);
        delete dummy_impl;
    }, this);
}


void DummyMachine::messageRecievedFromImpl(const MachineMessage &msg)
{

}

}//namespace r64fx
