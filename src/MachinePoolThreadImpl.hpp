#ifndef R64FX_MACHINE_POOL_IMPL_HPP
#define R64FX_MACHINE_POOL_IMPL_HPP

#include "CircularBuffer.hpp"

namespace r64fx{

class MachinePoolThreadImpl{
    friend class MachineImpl;
    
    CircularBuffer<MachineMessage>*  m_to_thread    = nullptr;
    CircularBuffer<MachineMessage>*  m_from_thread  = nullptr;
    bool                             m_running      = true;

    MachineImpl*  m_dst_impl   = nullptr;
    Machine*      m_dst_iface  = nullptr;
    
public:
    void run(CircularBuffer<MachineMessage>* to_thread, CircularBuffer<MachineMessage>* from_thread);
    
    void pickDestinationIface(Machine* dst);
    
    void sendMessagesToIface(Machine* dst, MachineMessage* msgs, int nmsgs);
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_IMPL_HPP
