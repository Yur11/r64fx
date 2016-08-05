#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"
#include "LinkedList.hpp"

namespace r64fx{
    
class MachineManagerImpl;
class MachineGlobalContext;
    
class MachineImpl : public LinkedList<MachineImpl>::Node{
    friend MachineManagerImpl;
    
    MachineManagerImpl* m_manager_impl = nullptr;
    Machine* m_iface = nullptr;
    MachineGlobalContext* m_ctx = nullptr;
    
    inline void setManagerImpl(MachineManagerImpl* manager)
    {
        m_manager_impl = manager;
    }
    
    inline void setContext(MachineGlobalContext* ctx)
    {
        m_ctx = ctx;
    }
    
public:
    MachineImpl(Machine* iface);
    
    virtual ~MachineImpl();
    
    inline MachineManagerImpl* managerImpl() const
    {
        return m_manager_impl;
    }

    inline Machine* iface() const 
    { 
        return m_iface; 
    }
    
    inline MachineGlobalContext* ctx() const
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