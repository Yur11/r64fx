#include "Module_SoundDriver.hpp"
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{

struct SoundDriverDeploymentArgs{};

struct SoundDriverWithdrawalArgs{};

R64FX_DECL_MODULE_AGENTS(SoundDriver)


enum{
    AddAudioInput,
    AddAudioOutput,
    RemoveAudioInput,
    RemoveAudioOutput
};

class SoundDriverAudioInputHandle;
class SoundDriverAudioOutputHandle;

struct Message_AddAudioInput{
    inline static unsigned long key() { return AddAudioInput; }

    SoundDriverAudioInput*        sd_port   = nullptr;
    Response_AddAudioInput*       response  = nullptr;
    void*                         arg1      = nullptr;
    void*                         arg2      = nullptr;
    SoundDriverAudioInputHandle*  handle    = nullptr;
};

struct Message_AddAudioOutput{
    inline static unsigned long key() { return AddAudioOutput; }

    SoundDriverAudioOutput*        sd_port   = nullptr;
    Response_AddAudioOutput*       response  = nullptr;
    void*                          arg1      = nullptr;
    void*                          arg2      = nullptr;
    SoundDriverAudioOutputHandle*  handle    = nullptr;
};


/*======= Worker Thread =======*/

class SoundDriverAudioInputImpl : public LinkedList<SoundDriverAudioInputImpl>::Node{
    friend class SoundDriverThreadObjectImpl;
    SoundDriverAudioInput*  sd_port  = nullptr;
    float*                  buffer   = nullptr;
};

class SoundDriverAudioOutputImpl : public LinkedList<SoundDriverAudioOutputImpl>::Node{
    friend class SoundDriverThreadObjectImpl;
    SoundDriverAudioOutput*  sd_port  = nullptr;
    float*                   buffer   = nullptr;
};


class SoundDriverThreadObjectImpl : public ModuleThreadObjectImpl{
    LinkedList<SoundDriverAudioInputImpl> m_inputs;
    LinkedList<SoundDriverAudioOutputImpl> m_outputs;

public:
    SoundDriverThreadObjectImpl(SoundDriverDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
    }

    ~SoundDriverThreadObjectImpl()
    {
    }

    void storeWithdrawalArgs(SoundDriverWithdrawalAgent* agent)
    {
    }

    template<typename MessageT, typename ImplT, typename HandleT>
    inline void messageAddAudioPort(const ThreadObjectMessage &msg, LinkedList<ImplT> &impl_list)
    {
        auto message = (MessageT*)msg.value();
        auto impl = new ImplT;
        impl->sd_port = message->sd_port;
        impl->buffer = new float[bufferSize()];
        message->handle = (HandleT*) impl;
        sendMessagesToIface(&msg, 1);
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
#define R64FX_IMPL_MESSAGE_ADD_AUDIO_PORT(type, list) case Add##type:\
    { messageAddAudioPort <Message_Add##type, SoundDriver##type##Impl, SoundDriver##type##Handle> (msg, list); break; }
            R64FX_IMPL_MESSAGE_ADD_AUDIO_PORT(AudioInput,  m_inputs)
            R64FX_IMPL_MESSAGE_ADD_AUDIO_PORT(AudioOutput, m_outputs)
#undef R64FX_IMPL_MESSAGE_ADD_AUDIO_PORT

            default:
            {
                std::cerr << "Bad Message from iface!\n";
                break;
            }
        }
    }

    inline void prologue()
    {

    }

    inline void epilogue()
    {

    }
};

R64FX_DEF_MODULE_AGENTS(SoundDriver)



/*======= Main Thread =======*/

class SoundDriverThreadObjectIface : public ModuleThreadObjectIface{
public:
    ModuleCallback  done      = nullptr;
    void*           done_arg  = nullptr;

private:
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
    {
        auto agent = new SoundDriverDeploymentAgent;
        return agent;
    }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
    {
        return new SoundDriverWithdrawalAgent;
    }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
    {
        auto withdrawal_agent = static_cast<SoundDriverWithdrawalAgent*>(agent);
        delete withdrawal_agent;
    }

    template<typename MessageT, typename ResponseT, typename SDPortT>
    inline void addPort(const char* name, ResponseT* response, void* arg1, void* arg2)
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG
        SDPortT* sd_port = nullptr;
        sd->newPort(&sd_port, name);
#ifdef R64FX_DEBUG
        assert(sd_port != nullptr);
#endif//R64FX_DEBUG

        auto message = new MessageT;
        message->sd_port = sd_port;
        message->response = response;
        message->arg1 = arg1;
        message->arg2 = arg2;
        ThreadObjectMessage msg(MessageT::key(), &message);
        sendMessagesToImpl(&msg, 1);
    }

public:
#define R64FX_ADD_PORT(type, name, response, arg1, arg2) addPort<Message_Add##type, Response_Add##type, SoundDriver##type>(name, response, arg1, arg2)
    inline void addAudioInput(const char* name, Response_AddAudioInput* response, void* arg1, void* arg2)
    {
        R64FX_ADD_PORT(AudioInput, name, response, arg1, arg2);
    }

    inline void addAudioOutput(const char* name, Response_AddAudioOutput* response, void* arg1, void* arg2)
    {
        R64FX_ADD_PORT(AudioOutput, name, response, arg1, arg2);
    }
#undef R64FX_ADD_PORT

private:
    template<typename MessageT, typename HandleT>
    inline void responseAddAudioPort(const ThreadObjectMessage &msg)
    {
        auto message = (MessageT*) msg.value();
        if(message->response)
        {
            message->response(nullptr, message->arg1, message->arg2);
        }
        delete message;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        switch(msg.key())
        {
#define R64FX_RESPONSE_ADD_AUDIO_PORT(type) case Add##type: { responseAddAudioPort<Message_Add##type, SoundDriver##type##Handle>(msg);  break; }
            R64FX_RESPONSE_ADD_AUDIO_PORT(AudioInput)
            R64FX_RESPONSE_ADD_AUDIO_PORT(AudioOutput)
#undef R64FX_RESPONSE_ADD_AUDIO_PORT

            default:
            {
                std::cerr << "Bad message from impl!\n";
                break;
            }
        }
    }
};

}//namespace

#define m_thread_object_iface ((SoundDriverThreadObjectIface*)m)


Module_SoundDriver::Module_SoundDriver()
{
    
}


Module_SoundDriver::~Module_SoundDriver()
{
    
}


bool Module_SoundDriver::engage(ModuleCallback done, void* done_arg)
{
    if(!m)
        m = new(std::nothrow) SoundDriverThreadObjectIface;
    if(!m)
        return false;
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG

    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        auto filter_iface = static_cast<SoundDriverThreadObjectIface*>(iface);
        if(filter_iface->done)
        {
            filter_iface->done((Module*)arg, filter_iface->done_arg);
        }
    }, this);
    return true;
}


void Module_SoundDriver::disengage(ModuleCallback done, void* done_arg)
{
    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        auto filter_iface = static_cast<SoundDriverThreadObjectIface*>(iface);
        if(filter_iface->done)
        {
            filter_iface->done((Module*)arg, filter_iface->done_arg);
        }
    }, this);
}


bool Module_SoundDriver::isEngaged()
{
    return m != nullptr && m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_SoundDriver::engagementPending()
{
    return m_thread_object_iface->isPending();
}


void Module_SoundDriver::addAudioInput(const char* name, Response_AddAudioInput* response, void* arg1, void* arg2)
{
    m_thread_object_iface->addAudioInput(name, response, arg1, arg2);
}


void Module_SoundDriver::addAudioOutput(const char* name, Response_AddAudioOutput* response, void* arg1, void* arg2)
{
    m_thread_object_iface->addAudioOutput(name, response, arg1, arg2);
}

}//namespace r64fx
