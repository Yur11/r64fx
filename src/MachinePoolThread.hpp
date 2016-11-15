#ifndef R64FX_MACHINE_POOL_THREAD_HPP
#define R64FX_MACHINE_POOL_THREAD_HPP

#include "MachineIface.hpp"
#include "Thread.hpp"
#include "CircularBuffer.hpp"
#include "MachineMessage.hpp"

namespace r64fx{

class MachineImpl;
class MachinePoolThreadImpl;

typedef MachineImpl*  (*MachineImplDeploymentFun) (MachineIface* iface,  MachinePoolThreadImpl* thread);
typedef void          (*MachineImplWithdrawalFun) (MachineImpl* impl,    MachinePoolThreadImpl* thread);

class MachinePoolThread : public LinkedList<MachinePoolThread>::Node, private Thread{
    friend class MachineIface;

    CircularBuffer<MachineMessage>*  m_to_thread    = nullptr;
    CircularBuffer<MachineMessage>*  m_from_thread  = nullptr;

    MachineImpl*   m_dst_impl   = nullptr;
    MachineIface*  m_dst_iface  = nullptr;

public:
    MachinePoolThread();

    ~MachinePoolThread();

    void start();

    void join();

    void readMessagesFromImpl();

    void deployImpl(MachineImplDeploymentFun fun, MachineIface* iface);
    
    void withdrawImpl(MachineImplWithdrawalFun fun, MachineImpl* impl);
    
private:
    void pickDestinationImpl(MachineImpl* dst_impl);

    void sendMessagesToImpl(MachineImpl* dst_impl, MachineMessage* msgs, int nmsgs);
};

}//namespace r64fx

#endif//R64FX_MACHINE_POOL_THREAD_HPP
