#ifndef R64FX_MACHINE_HPP
#define R64FX_MACHINE_HPP

#include <string>
#include "MachineMessage.hpp"
#include "MachinePort.hpp"

namespace r64fx{
    
class MachinePool;
class MachineImpl;
class MachinePoolThread;
class MachinePoolThreadImpl;

typedef MachineImpl*  (*MachineDeploymentFun) (Machine* iface, MachinePoolThreadImpl*);
typedef void          (*MachineWithdrawalFun) (MachineImpl* impl);

class Machine : public LinkedList<Machine>::Node{
    friend class MachinePool;
    friend class MachinePoolThread;

    MachinePool*        m_parent_pool        = nullptr;
    MachineImpl*        m_deployed_impl      = nullptr;
    MachinePoolThread*  m_deployment_thread  = nullptr;
    
    MachineDeploymentFun m_deploy   = nullptr;
    MachineWithdrawalFun m_withdraw = nullptr;

protected:
    Machine(MachinePool* parent_pool, MachineDeploymentFun deploy, MachineWithdrawalFun withdraw);
    
    virtual ~Machine();

public: 
    MachinePool* parentPool() const;
    
    void deploy();
    
    void withdraw();
    
    bool isDeployed() const;
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg) = 0;
    
private:
    void implDeployed(MachineImpl* impl);
    
    virtual void messageRecievedFromImpl(const MachineMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_MACHINE_HPP
