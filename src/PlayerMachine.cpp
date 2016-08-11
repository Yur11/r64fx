#include "PlayerMachine.hpp"
#include "MachineImpl.hpp"
#include "MachinePoolContext.hpp"
#include "SignalNode_Sampler.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
namespace{
    constexpr unsigned long SetComponentCount = 1;
    constexpr unsigned long SetFrameCount     = 2;
    constexpr unsigned long SetData           = 3;
    constexpr unsigned long ReturnSignalSource = 4;
}//namespace
    
class PlayerMachineImpl : public MachineImpl{
    int m_frame_count = 0;
    int m_component_count = 0;
    
    SignalNode_Sampler* m_sampler = nullptr;
    
public:    
    virtual void deploy()
    {
        
    }
    
    virtual void withdraw()
    {
        
    }
    
    void playData(float* data, int frame_count, int component_count)
    {
        cout << "playData: " << data << ", " << frame_count << ", " << component_count << "\n";
        if(!m_sampler)
        {
            m_sampler = new SignalNode_Sampler(data, frame_count, component_count, ctx()->signal_graph);
            sendMessage(ReturnSignalSource, (unsigned long) m_sampler->source(0));
        }
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        if(msg.opcode == SetComponentCount)
        {
            m_component_count = (int) msg.value;
        }
        else if(msg.opcode == SetFrameCount)
        {
            m_frame_count = (int) msg.value;
        }
        else if(msg.opcode == SetData)
        {
            float* data = (float*) msg.value;
            if(data && m_frame_count && m_component_count > 0)
            {
                playData(data, m_frame_count, m_component_count);
            }
        }
    }
};
    
    
PlayerMachine::PlayerMachine(MachinePool* pool)
: Machine(pool)
, m_signal_source(this, "out")
{
    setImpl(new PlayerMachineImpl);
}


PlayerMachine::~PlayerMachine()
{
    
}


void PlayerMachine::playData(float* data, int frame_count, int component_count)
{
    MachineMessage msgs[3] = {
        {SetComponentCount, (unsigned long) component_count},
        {SetFrameCount,     (unsigned long) frame_count},
        {SetData,           (unsigned long) data}
    };
    
    sendMessages(msgs, 3);
}


MachineSignalSource* PlayerMachine::signalSource()
{
    return &m_signal_source;
}


void PlayerMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{
    
}


void PlayerMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == ReturnSignalSource)
    {
        m_signal_source.setHandle((void*)msg.value);
    }
}
    
}//namespace r64fx