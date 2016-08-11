#include "PlayerMachine.hpp"
#include "MachineImpl.hpp"

namespace r64fx{
    
class PlayerMachineImpl : public MachineImpl{
public:    
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
    
    
PlayerMachine::PlayerMachine(MachinePool* pool)
: Machine(pool)
{
    setImpl(new PlayerMachineImpl);
}


PlayerMachine::~PlayerMachine()
{
    
}


void PlayerMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{
    
}


void PlayerMachine::dispatchMessage(const MachineMessage &msg)
{
    
}
    
}//namespace r64fx