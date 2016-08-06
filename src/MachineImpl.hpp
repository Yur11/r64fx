#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"
#include "LinkedList.hpp"

namespace r64fx{
    
class MachineThread;
class MachinePoolContext;
    
class MachineImpl : public LinkedList<MachineImpl>::Node{
    friend MachineThread;
    
    MachineThread* m_pool_impl = nullptr;
    Machine* m_iface = nullptr;
    MachinePoolContext* m_ctx = nullptr;
    
    inline void setPoolImpl(MachineThread* manager)
    {
        m_pool_impl = manager;
    }
    
    inline void setContext(MachinePoolContext* ctx)
    {
        m_ctx = ctx;
    }
    
public:
    MachineImpl(Machine* iface);
    
    virtual ~MachineImpl();
    
    inline MachineThread* poolImpl() const
    {
        return m_pool_impl;
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
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_IMPL_HPP