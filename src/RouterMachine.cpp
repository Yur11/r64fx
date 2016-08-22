#include "RouterMachine.hpp"
#include "MachineImpl.hpp"

namespace r64fx{

namespace{
    RouterMachine* g_router_machine_singleton_instance = nullptr;
}//namespace
    
    
class RouterMachineImpl : public MachineImpl{
    virtual void deploy()
    {
        
    }
    
    virtual void withdraw()
    {
        
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        
    }
};
    
    
RouterMachine::RouterMachine(MachinePool* pool)
: Machine(pool)
{
    setImpl(new RouterMachineImpl);
}
    

RouterMachine::~RouterMachine()
{
    
}


RouterMachine* RouterMachine::singletonInstance(MachinePool* pool)
{
    if(!g_router_machine_singleton_instance)
    {
        g_router_machine_singleton_instance = new RouterMachine(pool);
    }
    return g_router_machine_singleton_instance;
}


void RouterMachine::cleanup()
{
    if(g_router_machine_singleton_instance)
    {
        delete g_router_machine_singleton_instance;
    }
}


void RouterMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{
    
}


void RouterMachine::dispatchMessage(const MachineMessage &msg)
{
    
}
    
}//namespace r64fx