#include "Module_Filter.hpp"
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

struct FilterDeploymentArgs{
    SoundDriverAudioOutput* audio_output_port = nullptr;
};

struct FilterWithdrawalArgs{
    SoundDriverAudioOutput* audio_output_port = nullptr;
};

R64FX_DECL_MODULE_AGENTS(Filter)


struct FilterCoeffs{
    float*  input_coeffs        = nullptr;
    int     input_coeff_count   = 0;
    float*  output_coeffs       = nullptr;
    int     output_coeff_count  = 0;
};


/*======= Worker Thread =======*/

class FilterThreadObjectImpl : public ModuleThreadObjectImpl{
    SoundDriverAudioOutput* m_audio_output_port = nullptr;
    SoundDriverMidiInput*   m_midi_input_port   = nullptr;
    FilterCoeffs*           m_coeffs            = nullptr;

public:
    FilterThreadObjectImpl(FilterDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        m_audio_output_port  = agent->audio_output_port;

        setPrologue([](void* arg){
            auto self = (FilterThreadObjectImpl*) arg;
            self->prologue();
        }, this);

        setEpilogue([](void* arg){
            auto self = (FilterThreadObjectImpl*) arg;
            self->epilogue();
        }, this);
    }

    ~FilterThreadObjectImpl()
    {
    }

    void storeWithdrawalArgs(FilterWithdrawalAgent* agent)
    {
        agent->audio_output_port  = m_audio_output_port;
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        cout << "msg: " << long(msg.value()) << "\n";
    }

    inline void prologue()
    {
        float val = 0.0f;
        for(int i=0; i<bufferSize(); i++)
        {
            val = (i < bufferSize()/2 ? -1.0f : + 1.0f) * 0.25f;
            m_audio_output_port->writeSamples(&val, 1);
        }

        if(m_coeffs)
        {
            
        }
    }

    inline void epilogue()
    {

    }
};

R64FX_DEF_MODULE_AGENTS(Filter)



/*======= Main Thread =======*/

class FilterThreadObjectIface : public ModuleThreadObjectIface{
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

        auto filter_agent = new FilterDeploymentAgent;
        filter_agent->audio_output_port = sd->newAudioOutput("filter_out");
#ifdef R64FX_DEBUG
        assert(filter_agent->audio_output_port != nullptr);
#endif//R64FX_DEBUG
        sd->connect("r64fx:filter_out", "system:playback_1");
        sd->connect("r64fx:filter_out", "system:playback_2");
        return filter_agent;
    }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
    {
        return new FilterWithdrawalAgent;
    }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG

        auto filter_agent = static_cast<FilterWithdrawalAgent*>(agent);
        sd->deletePort(filter_agent->audio_output_port);

        delete filter_agent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {

    }
};

#define m_thread_object_iface ((FilterThreadObjectIface*)m)


Module_Filter::Module_Filter()
{

}


Module_Filter::~Module_Filter()
{

}


void Module_Filter::setFilterClass(FilterClass* fc)
{
    
}


bool Module_Filter::engage(ModuleCallback done, void* done_arg)
{
    if(!m)
        m = new(std::nothrow) FilterThreadObjectIface;
    if(!m)
        return false;
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG

    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        auto filter_iface = static_cast<FilterThreadObjectIface*>(iface);
        if(filter_iface->done)
        {
            filter_iface->done((Module*)arg, filter_iface->done_arg);
        }
    }, this);
    return true;
}


void Module_Filter::disengage(ModuleCallback done, void* done_arg)
{
    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        auto filter_iface = static_cast<FilterThreadObjectIface*>(iface);
        if(filter_iface->done)
        {
            filter_iface->done((Module*)arg, filter_iface->done_arg);
        }
    }, this);
}


bool Module_Filter::isEngaged()
{
    return m != nullptr && m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_Filter::engagementPending()
{
    return m_thread_object_iface->isPending();
}

}//namespace r64fx
