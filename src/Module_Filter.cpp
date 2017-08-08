#include "Module_Filter.hpp"
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"

#include <iostream>
#include <cmath>
#include <limits>

using namespace std;

namespace r64fx{

struct FilterDeploymentArgs{
    SoundDriverAudioInput*  audio_input_port    = nullptr;
    SoundDriverAudioOutput* audio_output_port   = nullptr;
};

struct FilterWithdrawalArgs{
    SoundDriverAudioInput*   audio_input_port   = nullptr;
    SoundDriverAudioOutput*  audio_output_port  = nullptr;
};

R64FX_DECL_MODULE_AGENTS(Filter)


struct SecondOrderCoeffs{
    long    size      = 0;
    float*  a0buff    = nullptr;
    float*  ab12buff  = nullptr;
};

enum{
    ChangeCoeffs,
    FreeCoeffs
};


/*======= Worker Thread =======*/

class FilterThreadObjectImpl : public ModuleThreadObjectImpl{
    SoundDriverAudioInput* m_audio_input_port  = nullptr;
    SoundDriverAudioOutput* m_audio_output_port = nullptr;
    SoundDriverMidiInput*   m_midi_input_port   = nullptr;

    SecondOrderCoeffs* m_soc = nullptr;
    float* m_buffer = nullptr;
    int m_counter = 0;

public:
    FilterThreadObjectImpl(FilterDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        m_audio_input_port   = agent->audio_input_port;
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
        agent->audio_input_port   = m_audio_input_port;
        agent->audio_output_port  = m_audio_output_port;
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
            case ChangeCoeffs:
            {
                int old_buff_size = 0;
                if(m_soc)
                {
                    old_buff_size = m_soc->size;
                    ThreadObjectMessage msg(FreeCoeffs, m_soc);
                    sendMessagesToIface(&msg, 1);
                }

                auto new_soc = (SecondOrderCoeffs*) msg.value();
                if(new_soc->size > old_buff_size)
                {
                    if(m_buffer)
                        delete m_buffer;

                    int buff_size = (new_soc->size + 1) << 1;
                    m_buffer = new float[buff_size];
                    for(int i=0; i<buff_size; i++)
                    {
                        m_buffer[i] = 0.0f;
                    }
                }
                m_soc = new_soc;
                break;
            }

            default:
            {
                std::cerr << "Bad Message from iface!\n";
                break;
            }
        }
    }

    float filter(float val)
    {
        if(!m_soc)
            return 0.0f;

        float* a0 = m_soc->a0buff;
        float* ab12 = m_soc->ab12buff;

        for(int i=0; i<m_soc->size; i++)
        {
            float* input = m_buffer + (i << 1);
            float* output = input + 2;

            float new_val = val * a0[i];
            new_val += input  [m_counter]      * ab12[(i << 2)];
            new_val += output [m_counter]      * ab12[(i << 2) | 2];
            new_val += input  [m_counter ^ 1]  * ab12[(i << 2) | 1];
            new_val += output [m_counter ^ 1]  * ab12[(i << 2) | 3];
            new_val = (new_val < -1.0f ? -1.0f : new_val);
            new_val = (new_val > +1.0f ? +1.0f : new_val);
            input  [m_counter ^ 1] = val;
            output [m_counter ^ 1] = new_val;
            val = new_val;
        }
        m_counter ^= 1;

        return val;
    }

    inline void prologue()
    {
        for(int i=0; i<bufferSize(); i++)
        {
            float val = 0.0f;
            m_audio_input_port->readSamples(&val, 1);
            val = filter(val);
            m_audio_output_port->writeSamples(&val, 1);
        }
    }

    inline void epilogue()
    {

    }
};

R64FX_DEF_MODULE_AGENTS(Filter)



/*======= Main Thread =======*/

class FilterThreadObjectIface : public ModuleThreadObjectIface{
    FilterClass* m_fc;

public:
    ModuleCallback  done      = nullptr;
    void*           done_arg  = nullptr;

    void setFilterClass(FilterClass* fc)
    {
        m_fc = fc;

        int zero_pair_count = 0;
        for(auto zero : m_fc->zeros())
        {
#ifdef R64FX_DEBUG
            assert(zero->hasConjugate());
#endif//R64FX_DEBUG
            zero_pair_count++;
        }

        int pole_pair_count = 0;
        for(auto pole : m_fc->poles())
        {
#ifdef R64FX_DEBUG
            assert(pole->hasConjugate());
#endif//R64FX_DEBUG
            pole_pair_count++;
        }

#ifdef R64FX_DEBUG
        assert(zero_pair_count == pole_pair_count);
#endif//R64FX_DEBUG

        SecondOrderCoeffs* soc = new SecondOrderCoeffs;
        soc->size      = zero_pair_count;
        soc->a0buff    = new float[zero_pair_count];
        soc->ab12buff  = new float[pole_pair_count << 2];

        int n = 0;
        for(auto zero : m_fc->zeros())
        {
            soc->a0buff[n] = 1.0f;

            float re = zero->value().re;
            float im = zero->value().im;
            soc->ab12buff[(n << 2)] = re;
            soc->ab12buff[(n << 2) | 1] = -(re*re + im*im);

            n++;
        }

        n = 0;
        for(auto pole : m_fc->poles())
        {
            soc->a0buff[n] = 1.0f;

            float re = pole->value().re;
            float im = pole->value().im;
            soc->ab12buff[(n << 2) | 2] = re;
            soc->ab12buff[(n << 2) | 3] = -(re*re + im*im);

            n++;
        }

        ThreadObjectMessage msg(ChangeCoeffs, soc);
        sendMessagesToImpl(&msg, 1);
    }

private:
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG

        auto filter_agent = new FilterDeploymentAgent;

        filter_agent->audio_input_port = sd->newAudioInput("filter_in");
#ifdef R64FX_DEBUG
        assert(filter_agent->audio_input_port != nullptr);
#endif//R64FX_DEBUG

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
        switch(msg.key())
        {
            case FreeCoeffs:
            {
                auto soc = (SecondOrderCoeffs*) msg.value();
                delete soc->a0buff;
                delete soc->ab12buff;
                delete soc;
                break;
            }

            default:
            {
                std::cerr << "Bad message from impl!\n";
                break;
            }
        }
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
    if(isEngaged())
    {
        m_thread_object_iface->setFilterClass(fc);
    }
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
