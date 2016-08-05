#include "DummyMachine.hpp"
#include "MachineImpl.hpp"
#include "sleep.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
namespace{
    constexpr unsigned long SayHello = 1;
}//namespace
    
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
        if(msg.opcode == SayHello)
        {
            sayHello(msg.value);
        }
    }
    
private:
    void sayHello(int num)
    {
        sendMessage(SayHello, 456);
        for(int i=0; i<100; i++)
        {
            cout << "a" << i << "\n";
            sleep_microseconds(10000);
        }
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


void DummyMachine::sayHello()
{
    sendMessage(MachineMessage(SayHello, 123));
}


void DummyMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == SayHello)
    {
        for(int i=0; i<100; i++)
        {
            cout << "b" << i << "\n";
            sleep_microseconds(10000);
        }
    }
}
    
}//namespace r64fx