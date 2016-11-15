#include "MachineImpl.hpp"
#include "MachinePoolThreadImpl.hpp"

namespace r64fx{

inline void MachineImpl::sendMessagesToIface(MachineMessage* msgs, int nmsgs)
{
    m_deployment_thread_impl->sendMessagesToIface(m_iface, msgs, nmsgs);
}

}//namespace r64fx
