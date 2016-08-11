#include "RouterMachine.hpp"
#include "MachineImpl.hpp"

namespace r64fx{
    
namespace{
    constexpr unsigned long SetSource = 1;
    constexpr unsigned long SetSink   = 2;
    constexpr unsigned long MakeSignalConnection     = 3;
    constexpr unsigned long MakeSequencerConnection  = 4;
}//namespace
    
class RouterMachineImpl : public MachineImpl{
    void* src = nullptr;
    void* dst = nullptr;
    
public:
    virtual void deploy()
    {
        
    }
    
    virtual void withdraw()
    {
        
    }
    
    void makeSignalConnection(void* src, void* dst)
    {
        
    }
    
    void makeSequencerConnection(void* src, void* dst)
    {
        
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        if(msg.opcode == SetSource)
        {
            src = (void*) msg.value;
        }
        else if(msg.opcode == SetSink)
        {
            dst = (void*) msg.value;
        }
        else if(msg.opcode == MakeSignalConnection)
        {
            if(src || dst)
            {
                makeSignalConnection(src, dst);
            }
        }
        else if(msg.opcode == MakeSequencerConnection)
        {
            if(src || dst)
            {
                makeSequencerConnection(src, dst);
            }
        }
    }
};
    

MachineConnection::MachineConnection(MachineSignalSource* src_port, MachineSignalSink* dst_port)
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


MachineConnection* RouterMachine::makeConnection(MachineSignalSource* src_port, MachineSignalSink* dst_port)
{
    MachineMessage msgs[3] = {
        {SetSource, (unsigned long) src_port->handle()},
        {SetSink,   (unsigned long) dst_port->handle()},
        {MakeSignalConnection, 0}
    };
    
    sendMessages(msgs, 3);
    auto connection = new MachineConnection(src_port, dst_port);
    m_connections.append(connection);
    return connection;
}
    
    
void RouterMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{
    
}
    
    
void RouterMachine::dispatchMessage(const MachineMessage &msg)
{
    
}
    
}//namespace r64fx