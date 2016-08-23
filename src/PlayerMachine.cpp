#include "PlayerMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineFlags.hpp"
#include "MachinePoolContext.hpp"
#include "MachinePortImpl.hpp"
#include "SignalNode_Sampler.hpp"
#include "RouterMachine.hpp"

#include "Timer.hpp"
#include "sleep.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
namespace{
    enum{
        ReplaceSample  = 1,
        Play,
        Stop,
        SetPitch,
        SetGain,
    
        SampleReplaced,
        SampleNotReplaced,
        OutputSizeChanged,
        PlayheadMoved
    };
    
    union MsgVal{
    private:
        unsigned long mulong = 0;
        float mf[2];
        
    public:
        explicit MsgVal(unsigned long ulong) : mulong(ulong) {}
        
        explicit MsgVal(float f0, float f1 = 0.0f) : mf{f0, f1} {}
        
        MsgVal() {}
        
        inline unsigned long ulong() const { return mulong; }
        
        inline operator unsigned long() const { return mulong; }
        
        inline float f(int i = 0) const { return mf[i]; }
    };
}//namespace
    
    
class PlayerMachineImpl : public MachineImpl{    
    SignalNode_Sampler* m_sampler = nullptr;
    SoundFileData* m_data = nullptr;
    MachineSourceImpl* m_output_impl = nullptr;
    float m_playhead_position = 0.0f;
    
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
            m_sampler->setPlayheadPosition(0.0f);
        }
        else if(msg.opcode == SetPitch)
        {
            m_sampler->setPitch(MsgVal(msg.value).f(0));
        }
        else if(msg.opcode == SetGain)
        {
            m_sampler->setGain(MsgVal(msg.value).f(0));
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
    
    virtual void cycleStarted()
    {
        
    }
    
    virtual void cycleEnded()
    {
        if(m_sampler->playheadPosition() != m_playhead_position)
        {
            m_playhead_position = m_sampler->playheadPosition();
            sendMessage(PlayheadMoved, MsgVal(m_playhead_position));
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


void PlayerMachine::setPitch(float pitch)
{
    m_pitch = pitch;
    sendMessage(SetPitch, MsgVal(pitch));
}


float PlayerMachine::pitch() const
{
    return m_pitch;
}


void PlayerMachine::setGain(float gain)
{
    m_gain = gain;
    sendMessage(SetGain, MsgVal(gain));
}


float PlayerMachine::gain() const
{
    return m_gain;
}


void PlayerMachine::setPlayheadPosition(float playhead_position)
{
    m_playhead_position = playhead_position;
}


float PlayerMachine::playheadPosition() const
{
    return m_playhead_position;
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
        m_output.setImpl(impl);
    }
    else if(msg.opcode == SampleReplaced)
    {
        m_sample = m_new_sample;
        m_new_sample.clear();
        m_flags |= R64FX_MACHINE_IS_READY;
    }
    else if(msg.opcode == SampleNotReplaced)
    {
        m_flags |= R64FX_MACHINE_IS_READY;
    }
    else if(msg.opcode == PlayheadMoved)
    {
        m_playhead_position = MsgVal(msg.value).f(0);
        cout << "pp: " << m_playhead_position << "\n";
    }
}
    
}//namespace r64fx