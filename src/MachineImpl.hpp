#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"
#include "LinkedList.hpp"

namespace r64fx{
    
class Machine;
class MachineThread;
class MachinePoolContext;
    
class MachineImpl : public LinkedList<MachineImpl>::Node{
    friend class MachineThread;
    friend class Machine;
    
    MachineThread* m_thread = nullptr;
    Machine* m_iface = nullptr;
    MachinePoolContext* m_ctx = nullptr;
    
    inline void setThread(MachineThread* manager)
    {
        m_thread = manager;
    }
    
    inline void setIface(Machine* machine)
    {
        m_iface = machine;
    }
    
    inline void setContext(MachinePoolContext* ctx)
    {
        m_ctx = ctx;
    }
    
public:
    virtual ~MachineImpl() {}
    
    inline MachineThread* thread() const
    {
        return m_thread;
    }

    inline Machine* iface() const 
    { 
        return m_iface; 
    }
    
    inline MachinePoolContext* ctx() const
    {
        return m_ctx;
    }
    
protected:
    void sendMessage(unsigned long opcode, unsigned long value);
    
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(const MachineMessage* msgs, int nmsgs);
    
    virtual void deploy() = 0;
    
    virtual void withdraw() = 0;
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
    
    virtual void cycleStarted() = 0;
    
    virtual void cycleEnded() = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_IMPL_HPP
