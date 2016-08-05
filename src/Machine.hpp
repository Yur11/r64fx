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
    
    bool m_is_deployed = false;
    
public:
    Machine(MachineManager* manager);
    
    virtual ~Machine();
    
    void deploy();
    
    void withdraw();
    
    inline bool isDeployed() const { return m_is_deployed; }
    
protected:
    void setImpl(MachineImpl* impl);
    
    MachineImpl* impl() const;
    
    void sendMessage(unsigned long opcode, unsigned long value);
    
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(const MachineMessage* msgs, int nmsgs);
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_HPP