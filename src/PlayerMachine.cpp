#include "PlayerMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineFlags.hpp"
#include "MachinePoolThreadImpl.hpp"
#include "MachinePortImpl.hpp"
#include "SignalNode_Sampler.hpp"

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
        MovePlayhead,

        SampleReplaced,
        SampleNotReplaced,
        OutputSizeChanged,
        PlayheadMoved,
        PlaybackStarted,
        PlaybackStopped
    };
}//namespace


struct PlayerMachineImpl : public MachineImpl{
    SignalNode_Sampler* m_sampler = nullptr;
    SoundFileData* m_data = nullptr;
    MachineSourceImpl* m_output_impl = nullptr;
    float m_playhead_time = 0.0f;

    PlayerMachineImpl(MachineIface* machine_iface)
    : MachineImpl(machine_iface)
    {
//         m_sampler = new SignalNode_Sampler;
//         ctx()->main_subgraph->addItem(m_sampler);
//         m_output_impl = new MachineSourceImpl;
//         resizeOutput(1);
    }

    ~PlayerMachineImpl()
    {
//         m_sampler->removeFromGraph();
//         ctx()->main_subgraph->removeItem(m_sampler);
//         delete m_sampler;
//         delete m_output_impl;
    }

    virtual void messageRecievedFromIface(const MachineMessage &msg)
    {
        
    }

//     void replaceSample(SoundFileData* data)
//     {
//         if(data && data->isGood())
//         {
//             m_sampler->setData(data->data(), data->frameCount(), data->componentCount(), data->sampleRate());
//             m_sampler->enableLoop();
//             if(m_output_impl->sources.size() != (unsigned long)data->componentCount())
//             {
//                 resizeOutput(data->componentCount());
//             }
//             m_data = data;
//             sendMessage(SampleReplaced, (unsigned long)m_output_impl);
//         }
//         else
//         {
//             sendMessage(SampleNotReplaced, 0);
//         }
//     }

//     virtual void dispatchMessage(const MachineMessage &msg)
//     {
//         if(msg.opcode == ReplaceSample)
//         {
//             replaceSample((SoundFileData*)msg.value);
//         }
//         else if(msg.opcode == Play)
//         {
//             m_sampler->play();
//             sendMessage(PlaybackStarted, 0);
//         }
//         else if(msg.opcode == Stop)
//         {
//             m_sampler->stop();
//             sendMessage(PlaybackStopped, 0);
//         }
//         else if(msg.opcode == SetPitch)
//         {
//             m_sampler->setPitch(MsgVal(msg.value).f(0));
//         }
//         else if(msg.opcode == SetGain)
//         {
//             m_sampler->setGain(MsgVal(msg.value).f(0));
//         }
//         else if(msg.opcode == MovePlayhead)
//         {
//             m_sampler->setPlayheadTime(MsgVal(msg.value).f(0));
//         }
//     }
// 
//     void resizeOutput(int size)
//     {
//         m_sampler->resizeOutput(size);
//         m_output_impl->sources.resize(size);
//         for(int i=0; i<size; i++)
//         {
//             m_output_impl->sources.at(i) = m_sampler->output(i);
//         }
// 
//         sendMessage(OutputSizeChanged, (unsigned long)m_output_impl);
//     }
// 
//     virtual void cycleStarted()
//     {
// 
//     }
// 
//     virtual void cycleEnded()
//     {
//         if(m_sampler->playheadTime() != m_playhead_time)
//         {
//             m_playhead_time = m_sampler->playheadTime();
//             sendMessage(PlayheadMoved, MsgVal(m_playhead_time));
//         }
//     }
};

namespace{

MachineImpl* deploy_impl(MachineIface* iface, MachinePoolThreadImpl*)
{
    return new PlayerMachineImpl(iface);
}


void withdraw_impl(MachineImpl* impl)
{
    auto machine_impl = static_cast<PlayerMachineImpl*>(impl);
    delete machine_impl;
}

}//namespace


PlayerMachine::PlayerMachine(MachinePool* pool)
: MachineIface(pool, deploy_impl, withdraw_impl)
, m_output(this, "out")
{

}


PlayerMachine::~PlayerMachine()
{

}

void PlayerMachine::replaceSample(SoundFileDataPtr new_sample)
{
//     if(!new_sample)
//         return;
// 
// #ifdef R64FX_DEBUG
//     assert(isReady());
// #endif//R64FX_DEBUG
// 
//     m_flags &= ~R64FX_MACHINE_IS_READY;
//     m_new_sample = new_sample;
// 
//     MachineMessage msg(ReplaceSample, (unsigned long)new_sample->data());
// 
//     if(!m_sample || new_sample->componentCount() != m_sample->componentCount())
//     {
//         packMessage(msg);
//         RouterMachine::singletonInstance(pool())->packConnectionUpdatesFor(&m_output);
//         sendPack();
//     }
//     else
//     {
//         sendMessage(msg);
//     }
}


void PlayerMachine::play()
{
//     sendMessage(Play, 0);
}


void PlayerMachine::stop()
{
//     sendMessage(Stop, 0);
}


void PlayerMachine::setPitch(float pitch)
{
//     m_pitch = pitch;
//     sendMessage(SetPitch, MsgVal(pitch));
}


float PlayerMachine::pitch() const
{
    return m_pitch;
}


void PlayerMachine::setGain(float gain)
{
//     m_gain = gain;
//     sendMessage(SetGain, MsgVal(gain));
}


float PlayerMachine::gain() const
{
    return m_gain;
}


void PlayerMachine::setPlayheadTime(float playhead_time)
{
//     m_playhead_time = playhead_time;
//     sendMessage(MovePlayhead, MsgVal(m_playhead_time));
}


float PlayerMachine::playheadTime() const
{
    return m_playhead_time;
}


bool PlayerMachine::isPlaying() const
{
    return m_is_playing;
}


MachineSignalSource* PlayerMachine::output()
{
    return &m_output;
}


void PlayerMachine::forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg)
{
//     fun(&m_output, this, arg);
}


void PlayerMachine::messageRecievedFromImpl(const MachineMessage &msg)
{
    
}

// void PlayerMachine::dispatchMessage(const MachineMessage &msg)
// {
//     if(msg.opcode == OutputSizeChanged)
//     {
//         auto impl = (MachineSourceImpl*)msg.value;
//         m_output.setImpl(impl);
//     }
//     else if(msg.opcode == SampleReplaced)
//     {
//         m_sample = m_new_sample;
//         m_new_sample.clear();
//         m_flags |= R64FX_MACHINE_IS_READY;
//     }
//     else if(msg.opcode == SampleNotReplaced)
//     {
//         m_flags |= R64FX_MACHINE_IS_READY;
//     }
//     else if(msg.opcode == PlayheadMoved)
//     {
//         m_playhead_time = MsgVal(msg.value).f(0);
//     }
//     else if(msg.opcode == PlaybackStarted)
//     {
//         m_is_playing = true;
//     }
//     else if(msg.opcode == PlaybackStopped)
//     {
//         m_is_playing = false;
//     }
// }

}//namespace r64fx
