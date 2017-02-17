#include "Module_Oscillator.hpp"
#include "ModulePrivate.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "SoundDriver.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

struct OscillatorDeploymentArgs{
    SoundDriverAudioOutput* audio_output_port = nullptr;
};

struct OscillatorWithdrawalArgs{
    SoundDriverAudioOutput* audio_output_port = nullptr;
};

R64FX_DECL_MODULE_AGENTS(Oscillator)


/*======= Worker Thread =======*/

class OscillatorThreadObjectImpl : public ModuleThreadObjectImpl{
    SoundDriverAudioOutput* m_audio_output_port = nullptr;

public:
    OscillatorThreadObjectImpl(OscillatorDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        m_audio_output_port = agent->audio_output_port;

        setPrologue([](void* arg){
            auto self = (OscillatorThreadObjectImpl*) arg;
            self->prologue();
        }, this);

        setEpilogue([](void* arg){
            auto self = (OscillatorThreadObjectImpl*) arg;
            self->epilogue();
        }, this);
    }

    ~OscillatorThreadObjectImpl()
    {
    }

    void storeWithdrawalArgs(OscillatorWithdrawalAgent* agent)
    {
        agent->audio_output_port = m_audio_output_port;
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        cout << "msg: " << long(msg.value()) << "\n";
    }

    inline void prologue()
    {
        float buff[256];
        auto bs = bufferSize();
        auto hbs = bs/2;
        for(int i=0; i<bs; i++)
        {
            buff[i] = (i < hbs ? 1.0f : -1.0f) * 0.3f;
        }
        m_audio_output_port->writeSamples(buff, 256);
    }

    inline void epilogue()
    {

    }
};

R64FX_DEF_MODULE_AGENTS(Oscillator)


/*======= Main Thread =======*/

class OscillatorThreadObjectIface : public ModuleThreadObjectIface{
public:
    ModuleCallback  done      = nullptr;
    void*           done_arg  = nullptr;

private:
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG

        auto agent = new OscillatorDeploymentAgent;
        agent->audio_output_port = sd->newAudioOutput("osc_out");
#ifdef R64FX_DEBUG
        assert(agent->audio_output_port != nullptr);
#endif//R64FX_DEBUG
        sd->connect("r64fx:osc_out", "system:playback_1");
        sd->connect("r64fx:osc_out", "system:playback_2");
        return agent;
    }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
    {
        return new OscillatorWithdrawalAgent;
    }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG

        delete agent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        
    }
};

#define m_thread_object_iface ((OscillatorThreadObjectIface*)m)


Module_Oscillator::Module_Oscillator()
{

}


Module_Oscillator::~Module_Oscillator()
{

}


bool Module_Oscillator::engage(ModuleCallback done, void* done_arg)
{
    if(!m)
        m = new(std::nothrow) OscillatorThreadObjectIface;
    if(!m)
        return false;
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG

    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        auto osc_iface = static_cast<OscillatorThreadObjectIface*>(iface);
        if(osc_iface->done)
        {
            osc_iface->done((Module*)arg, osc_iface->done_arg);
        }
    }, this);
    return true;
}


void Module_Oscillator::disengage(ModuleCallback done, void* done_arg)
{
    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        auto osc_iface = static_cast<OscillatorThreadObjectIface*>(iface);
        if(osc_iface->done)
        {
            osc_iface->done((Module*)arg, osc_iface->done_arg);
        }
    }, this);
}


bool Module_Oscillator::isEngaged()
{
    return m != nullptr && m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_Oscillator::engagementPending()
{
    return m_thread_object_iface->isPending();
}

}//namespace r64fx
