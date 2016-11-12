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
class MachinePoolThreadImpl;

typedef MachineImpl*  (*MachineDeploymentFun) (MachineIface* iface, MachinePoolThreadImpl*);
typedef void          (*MachineWithdrawalFun) (MachineImpl* impl);

class MachineIface : public LinkedList<MachineIface>::Node{
    friend class MachinePool;
    friend class MachinePoolThread;

    MachinePool*        m_parent_pool        = nullptr;
    MachineImpl*        m_deployed_impl      = nullptr;
    MachinePoolThread*  m_deployment_thread  = nullptr;
    
    MachineDeploymentFun m_deploy   = nullptr;
    MachineWithdrawalFun m_withdraw = nullptr;

protected:
    MachineIface(MachinePool* parent_pool, MachineDeploymentFun deploy, MachineWithdrawalFun withdraw);
    
    virtual ~MachineIface();

public: 
    MachinePool* parentPool() const;
    
    void deploy();
    
    void withdraw();
    
    bool isDeployed() const;
    
    virtual void forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg) = 0;
    
private:
    void implDeployed(MachineImpl* impl);
    
    virtual void messageRecievedFromImpl(const MachineMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_MACHINE_IFACE_HPP
