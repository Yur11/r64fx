#ifndef R64FX_ROUTER_MACHINE_HPP
#define R64FX_ROUTER_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{
    
class RouterMachine : public Machine{
    RouterMachine(MachinePool* pool);
    
    virtual ~RouterMachine();
    
public:
    static RouterMachine* singletonInstance(MachinePool* pool);
    
    static void cleanup();
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
private:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_ROUTER_MACHINE_HPP