#include "PlayerMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineFlags.hpp"
#include "MachinePoolContext.hpp"
#include "MachineConnectionDatabase.hpp"
#include "MachinePortImpl.hpp"
#include "SignalNode_Sampler.hpp"
#include "RouterMachine.hpp"

#include "Timer.hpp"
#include "sleep.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
namespace{
    constexpr unsigned long ReplaceSample  = 1;
    constexpr unsigned long Play           = 2;
    constexpr unsigned long Stop           = 3;
    
    constexpr unsigned long SampleReplaced    = 4;
    constexpr unsigned long SampleNotReplaced = 5;
    constexpr unsigned long OutputSizeChanged = 6;
}//namespace
    
    
class PlayerMachineImpl : public MachineImpl{    
    SignalNode_Sampler* m_sampler = nullptr;
    SoundFileData* m_data = nullptr;
    MachineSourceImpl* m_output_impl = nullptr;
    
public:    
    virtual void deploy()
    {
        m_sampler = new SignalNode_Sampler;
        ctx()->main_subgraph->addItem(m_sampler);
        m_output_impl = new MachineSourceImpl;
        resizeOutput(1);
    }
    
    virtual void withdraw()
    {
        m_sampler->removeFromGraph();
        delete m_output_impl;
    }
        
    void replaceSample(SoundFileData* data)
    {
        if(data && data->isGood())
        {
            m_sampler->setData(data->data(), data->frameCount(), data->componentCount(), data->sampleRate());
            m_sampler->play();
            m_sampler->enableLoop();
            if(m_output_impl->sources.size() != (unsigned long)data->componentCount())
            {
                resizeOutput(data->componentCount());
            }
            m_data = data;
            sendMessage(SampleReplaced, (unsigned long)m_output_impl);
        }
        else
        {
            sendMessage(SampleNotReplaced, 0);
        }
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        if(msg.opcode == ReplaceSample)
        {
            replaceSample((SoundFileData*)msg.value);
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
    
    void resizeOutput(int size)
    {
        m_sampler->resizeOutput(size);
        m_output_impl->sources.resize(size);
        for(int i=0; i<size; i++)
        {
            m_output_impl->sources.at(i) = m_sampler->output(i);
        }
        
        sendMessage(OutputSizeChanged, (unsigned long)m_output_impl);
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

void PlayerMachine::replaceSample(SoundFileDataPtr new_sample)
{
    if(!new_sample)
        return;
    
    assert(isReady());
    
    m_flags &= ~R64FX_MACHINE_IS_READY;
    m_new_sample = new_sample;
    
    MachineMessage msg(ReplaceSample, (unsigned long)new_sample.data());
    
    if(!m_sample || new_sample.componentCount() != m_sample.componentCount())
    {
        packMessage(msg);
        RouterMachine::singletonInstance(pool())->packConnectionUpdatesFor(&m_output);
        sendPack();
    }
    else
    {
        sendMessage(msg);
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
    if(msg.opcode == OutputSizeChanged)
    {
        auto impl = (MachineSourceImpl*)msg.value;
        cout << "OutputSizeChanged: " << impl->sources.size() << "\n";
        m_output.setImpl(impl);
    }
    else if(msg.opcode == SampleReplaced)
    {
        m_sample = m_new_sample;
        m_new_sample.clear();
        m_flags |= R64FX_MACHINE_IS_READY;
        cout << "SampleReplaced\n";
    }
    else if(msg.opcode == SampleNotReplaced)
    {
        m_flags |= R64FX_MACHINE_IS_READY;
        cout << "SampleNotReplaced\n";
    }
}
    
}//namespace r64fx