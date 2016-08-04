#ifndef R64FX_MACHINE_IMPL_HPP
#define R64FX_MACHINE_IMPL_HPP

#include "MachineMessage.hpp"

namespace r64fx{
    
class MachineManagerImpl;
    
class MachineImpl{
    friend MachineManagerImpl;
    
    MachineManagerImpl* m_manger_impl = nullptr;
    Machine* m_machine = nullptr;
    
public:
    MachineImpl(MachineManagerImpl* manager_impl, Machine* machine);
    
    virtual ~MachineImpl();
    
    inline Machine* machine() const { return m_machine; }
    
protected:
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(const MachineMessage* msgs, int nmsgs);
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_IMPL_HPP