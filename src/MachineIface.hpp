#ifndef R64FX_MACHINE_IFACE_HPP
#define R64FX_MACHINE_IFACE_HPP

#include <string>
#include "MachineMessage.hpp"
#include "MachinePort.hpp"

namespace r64fx{

class MachineIface;
class MachineImpl;
class MachinePool;
class MachinePoolThread;

class MachineIface : public LinkedList<MachineIface>::Node{
    friend class MachinePool;
    friend class MachinePoolThread;

    MachinePool*        m_parent_pool        = nullptr;
    MachineImpl*        m_deployed_impl      = nullptr;
    MachinePoolThread*  m_deployment_thread  = nullptr;

protected:
    MachineIface();

    virtual ~MachineIface();

public: 
    MachinePool* parentPool() const;

    bool isDeployed() const;

    void withdraw();

    virtual void forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg) = 0;
    
protected:
    inline MachinePoolThread* deploymentThread() const 
    {
        return m_deployment_thread;
    }
    
    virtual void deploymentEvent() = 0;
    
    virtual void withdrawalEvent() = 0;

    void sendMessagesToImpl(MachineMessage* msgs, int nmsgs);
    
    virtual void messageRecievedFromImpl(const MachineMessage &msg) = 0;
    
private:
    void implDeployed(MachineImpl* impl);
};

}//namespace r64fx

#endif//R64FX_MACHINE_IFACE_HPP
