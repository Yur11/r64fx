#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"

namespace r64fx{
    
class MachineManagerImpl;
    
class MachineImpl{
    friend MachineManagerImpl;
    
    MachineManagerImpl* m_manager_impl = nullptr;
    Machine* m_iface = nullptr;
    
public:
    MachineImpl(MachineManagerImpl* manager_impl, Machine* iface);
    
    virtual ~MachineImpl();
    
    inline Machine* iface() const { return m_iface; }
    
protected:
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(const MachineMessage* msgs, int nmsgs);
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_IMPL_HPP