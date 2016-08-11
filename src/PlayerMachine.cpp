#include "PlayerMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineFlags.hpp"
#include "MachinePoolContext.hpp"
#include "MachineConnectionDatabase.hpp"
#include "MachinePortImpl.hpp"
#include "SignalNode_Sampler.hpp"

#include "Timer.hpp"
#include "sleep.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
namespace{
    constexpr unsigned long SetData     = 1;
    constexpr unsigned long Play        = 3;
    constexpr unsigned long Stop        = 4;
    
    constexpr unsigned long ReturnImpl  = 5;
}//namespace
    
    
class PlayerMachineImpl : public MachineImpl{    
    SignalNode_Sampler* m_sampler = nullptr;
    SignalData* m_data = nullptr;
    MachineSourceImpl* m_output_impl = nullptr;
    
public:    
    virtual void deploy()
    {
        m_sampler = new SignalNode_Sampler;
        ctx()->main_subgraph->addItem(m_sampler);
    }
    
    virtual void withdraw()
    {
        m_sampler->removeFromGraph();
    }
    
    void setData(SignalData* data)
    {
        if(data && data->isGood())
        {
            m_sampler->setData(data->data(), data->frameCount(), data->componentCount(), data->sampleRate());
            m_sampler->play();
            m_sampler->enableLoop();
            if(!m_data || m_data->componentCount() != data->componentCount())
            {
                m_sampler->resizeOutput(data->componentCount());
                if(m_output_impl)
                {
                    delete m_output_impl;
                }
                
                m_output_impl = new MachineSourceImpl(data->componentCount());
                for(int i=0; i<data->componentCount(); i++)
                {
                    m_output_impl->at(i) = m_sampler->output(i);
                }
                    
                sendMessage(ReturnImpl, (unsigned long)m_output_impl);
            }
            m_data = data;
        }        
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        if(msg.opcode == SetData)
        {
            setData((SignalData*)msg.value);
        }
        else if(msg.opcode == Play)
        {
            m_sampler->play();
        }
        else if(msg.opcode == Stop)
        {
            m_sampler->stop();
            m_sampler->setPlayHeadPosition(0.0f);
        }
    }
};
    
    
PlayerMachine::PlayerMachine(MachinePool* pool)
: Machine(pool)
, m_output(this, "out")
{
    setImpl(new PlayerMachineImpl);
}


PlayerMachine::~PlayerMachine()
{
    
}


void PlayerMachine::setData(SignalData* data)
{
    if(!data)
        return;

    auto data_copy = new SignalData(*data);
    MachineMessage msg(SetData, (unsigned long)data_copy);
    
    if(!m_data || data->componentCount() != m_data->componentCount())
    {
        m_output.setImpl(nullptr);
        packMessage(msg);
        packConnectionUpdatesFor(&m_output);
        sendPack();
    }
    else
    {
        sendMessage(msg);
    }
    m_data = data;
    
    while(!m_output.impl())
    {
        Timer::runTimers();
        sleep_microseconds(5000);
    }
}


void PlayerMachine::play()
{
    sendMessage(Play, 0);
}
    
    
void PlayerMachine::stop()
{
    sendMessage(Stop, 0);
}


MachineSignalSource* PlayerMachine::output()
{
    return &m_output;
}


void PlayerMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{
    fun(&m_output, this, arg);
}


void PlayerMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == ReturnImpl)
    {
        m_output.setImpl((MachineSourceImpl*)msg.value);
    }
}
    
}//namespace r64fx