#ifndef R64FX_MACHINE_HPP
#define R64FX_MACHINE_HPP

#include "MachineMessage.hpp"

namespace r64fx{
    
class MachineManager;
class MachineManagerPrivate;
class MachineImpl;
    
class Machine{
    friend class MachineManager;
    friend class MachineManagerPrivate;
    
    MachineManagerPrivate* m_manager_private = nullptr;
    MachineImpl* m_impl = nullptr;
    
public:
    Machine(MachineManager* manager);
    
    virtual ~Machine();
    
    
protected:
    inline void setImpl(MachineImpl* impl)
    {
        m_impl = impl;
    }
    
    inline MachineImpl* impl() const
    {
        return m_impl;
    }
    
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(const MachineMessage* msgs, int nmsgs);
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_HPP