#include "DummyMachine.hpp"
#include "MachineImpl.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
class DummyMachineImpl : public MachineImpl{
public:
    DummyMachineImpl(Machine* iface)
    : MachineImpl(iface)
    {
        
    }
    
    virtual ~DummyMachineImpl()
    {
        
    }
    
protected:
    virtual void deploy()
    {
        cout << "deploy\n";
    }
    
    virtual void withdraw()
    {
        cout << "withdraw\n";
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        
    }
};
    
    
DummyMachine::DummyMachine(MachineManager* manager)
: Machine(manager)
{
    auto impl = new DummyMachineImpl(this);
    setImpl(impl);
}
    
    
DummyMachine::~DummyMachine()
{
    
}


void DummyMachine::dispatchMessage(const MachineMessage &msg)
{
    
}
    
}//namespace r64fx