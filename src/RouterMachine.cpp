#include "RouterMachine.hpp"
#include "MachineImpl.hpp"

namespace r64fx{
    
class RouterMachineImpl : public MachineImpl{
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
    

RouterMachineRule::RouterMachineRule(MachineSignalSource* src_port, MachineSignalSink* dst_port)
: m_src_port(src_port)
, m_dst_port(dst_port)
{
    
}

    
RouterMachine::RouterMachine(MachinePool* pool)
: Machine(pool)
{
    
}
    
    
RouterMachine::~RouterMachine()
{
    
}


RouterMachineRule* RouterMachine::createRule(MachineSignalSource* src_port, MachineSignalSink* dst_port)
{
    auto rule = new RouterMachineRule(src_port, dst_port);
    m_rules.append(rule);
    return rule;
}
    
    
void RouterMachine::dispatchMessage(const MachineMessage &msg)
{
    
}
    
}//namespace r64fx