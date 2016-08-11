#ifndef R64FX_MACHINE_HPP
#define R64FX_MACHINE_HPP

#include <string>
#include "MachineMessage.hpp"
#include "MachinePort.hpp"

namespace r64fx{
    
class MachinePool;
class MachinePoolPrivate;
class MachineImpl;
    
class Machine : public LinkedList<Machine>::Node{
    friend class MachinePool;
    friend class MachinePoolPrivate;
    
    MachinePoolPrivate* m_pool_private = nullptr;
    MachineImpl* m_impl = nullptr;
    
    std::string m_name = "";

protected:
    unsigned long m_flags = 0;
    
public:
    Machine(MachinePool* pool);
    
    virtual ~Machine();
    
    MachinePool* pool() const;
    
    void deploy();
    
    void withdraw();
    
    bool isDeployed() const;
    
    bool deploymentPending() const;
    
    bool withdrawalPending() const;
    
    void setName(const std::string &name);
    
    std::string name() const;
    
    bool isReady() const;
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg) = 0;
    
protected:
    void setImpl(MachineImpl* impl);
    
    MachineImpl* impl() const;
    
    void sendMessage(unsigned long opcode, unsigned long value);
    
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(const MachineMessage* msgs, int nmsgs);
    
    void packMessage(unsigned long opcode, unsigned long value);
    
    void packMessage(const MachineMessage &msg);
    
    void packMessages(const MachineMessage* msgs, int nmsgs);
    
    void sendPack();
    
    void clearPack();
    
    int packSize() const;
    
    void packConnectionUpdatesFor(MachineSignalSource* source);
    
    void packConnectionUpdatesFor(MachineSignalSink* sink);
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
    
    static void block();
};


class MachineEventIface{
public:
    virtual void machineDeployed() = 0;
    
    virtual void machineWithdrawn() = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_HPP