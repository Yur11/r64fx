#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"

namespace r64fx{

class MachineIface;
class MachinePoolThreadImpl;

class MachineImpl{
    MachineIface*           m_iface                   = nullptr;
    MachinePoolThreadImpl*  m_deployment_thread_impl  = nullptr;

public:
    MachineImpl(MachineIface* machine_iface, MachinePoolThreadImpl* thread)
    : m_iface(machine_iface)
    , m_deployment_thread_impl(thread)
    {

    }

    virtual ~MachineImpl() {}

    void sendMessagesToIface(MachineMessage* msgs, int nmsgs);

    virtual void messageRecievedFromIface(const MachineMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_MACHINE_IMPL_HPP
