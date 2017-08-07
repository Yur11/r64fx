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
    float a0 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
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

    SecondOrderCoeffs* m_coeffs = nullptr;

    float input_buffer[2];
    float output_buffer[2];

    int buff_counter = 0;

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

        for(int i=0; i<2; i++)
        {
            input_buffer[i] = output_buffer[i] = 0.0f;
        }
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
                if(m_coeffs)
                {
                    ThreadObjectMessage msg(FreeCoeffs, m_coeffs);
                    sendMessagesToIface(&msg, 1);
                }
                m_coeffs = (SecondOrderCoeffs*) msg.value();

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
        if(!m_coeffs)
            return 0.0f;

        int n1 = (buff_counter + 0) & 1;
        int n2 = (buff_counter + 1) & 1;

        float result = 
            val * m_coeffs->a0 
            + input_buffer[n1] * m_coeffs->a1
            + input_buffer[n2] * m_coeffs->a2
            + output_buffer[n1] * m_coeffs->b1
            + output_buffer[n2] * m_coeffs->b2
        ;

        if(result > 1.0f)
            result = 1.0f;
        else if(result < -1.0f)
            result = -1.0f;

//         cout << val << " -> " << result << "\n";
        if(isinf(result))
            abort();
        input_buffer[n2] = val;
        output_buffer[n2] = result;

        buff_counter = n2;
        return result;
    }

    inline void prologue()
    {
//         static int n = 0;
//         static int m = 0;
        for(int i=0; i<bufferSize(); i++)
        {
            float val = 0.0f;
//             val = (n < 150 ? -1.0f : + 1.0f) * 0.125f + (m < 300 ? -1.0f : + 1.0f) * 0.125;
            m_audio_input_port->readSamples(&val, 1);
            val = filter(val);
            m_audio_output_port->writeSamples(&val, 1);

//             n++;
//             if(n >= 300)
//                 n = 0;
// 
//             m++;
//             if(m >= 600)
//                 m = 0;
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

        auto zero = *(fc->zeros().begin());
        auto pole = *(fc->poles().begin());
#ifdef R64FX_DEBUG
        assert(zero->hasConjugate());
        assert(pole->hasConjugate());
        assert(zero->hasValue());
        assert(pole->hasValue());
#endif//R64FX_DEBUG

        auto c = new SecondOrderCoeffs;
        c->a0 = 1.0f;
        c->a1 = zero->value().re;
        c->a2 = -(zero->value().re * zero->value().re + zero->value().im * zero->value().im);
        c->b1 = pole->value().re;
        c->b2 = -(pole->value().re * pole->value().re + pole->value().im * pole->value().im);

        ThreadObjectMessage msg(ChangeCoeffs, c);
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
                auto c = (SecondOrderCoeffs*) msg.value();
                delete c;
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
