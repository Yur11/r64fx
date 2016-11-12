#ifndef R64FX_MACHINE_POOL_THREAD_HPP
#define R64FX_MACHINE_POOL_THREAD_HPP

#include "MachineIface.hpp"
#include "Thread.hpp"
#include "CircularBuffer.hpp"
#include "MachineMessage.hpp"

namespace r64fx{

class MachineImpl;

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

private:
    void pickDestinationImpl(MachineImpl* dst);

    void sendMessagesToImpl(MachineImpl* dst, MachineMessage* msgs, int nmsgs);

    void deployMachine(MachineDeploymentFun fun, MachineIface* iface);
};

}//namespace r64fx

#endif//R64FX_MACHINE_POOL_THREAD_HPP
