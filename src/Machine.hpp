#ifndef R64FX_MACHINE_HPP
#define R64FX_MACHINE_HPP

#include <string>
#include "MachineMessage.hpp"
#include "MachinePort.hpp"

namespace r64fx{
    
class MachinePool;
class MachinePoolPrivate;
class MachineImpl;
    
class Machine{
    friend class MachinePool;
    friend class MachinePoolPrivate;
    
    MachinePoolPrivate* m_pool_private = nullptr;
    MachineImpl* m_impl = nullptr;
    
    bool m_is_deployed = false;
    
    std::string m_name = "";
    
public:
    Machine(MachinePool* pool);
    
    virtual ~Machine();
    
    void deploy();
    
    void withdraw();
    
    inline bool isDeployed() const { return m_is_deployed; }
    
    inline void setName(const std::string &name)
    {
        m_name = name;
    }
    
    inline std::string name() const
    {
        return m_name;
    }
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg) = 0;
    
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