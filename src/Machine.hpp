#ifndef R64FX_MACHINE_HPP
#define R64FX_MACHINE_HPP

#include "MachineMessage.hpp"

namespace r64fx{
    
class MachineManager;
class MachineImpl;
    
class Machine{
    friend class MachineManager;
    friend class MachineManagerPrivate;
    
    MachineManager* m_manager = nullptr;
    
public:
    Machine(MachineManager* manager);
    
    virtual ~Machine();
    
    virtual MachineImpl* impl() = 0;
    
protected:
    void sendMessage(const MachineMessage &msg);
    
    void sendMessages(MachineMessage* msgs, int nmsgs);
    
    virtual void dispatchMessage(const MachineMessage &msg) = 0;
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_HPP