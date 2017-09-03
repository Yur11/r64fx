#include "Module_SoundDriver.hpp"
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"
#include "SignalNode_BufferRW.hpp"

namespace r64fx{

namespace{

R64FX_DECL_DEFAULT_MODULE_AGENTS(SoundDriver);


template<unsigned long MsgKey, typename SDPortT, typename GraphPortT, typename ResponseT>
struct Message{
    inline constexpr static unsigned long Key() { return MsgKey; }

    Message(ResponseT* response, void* arg1, void* arg2)
    : response(response), arg1(arg1), arg2(arg2) {}

    SDPortT*     sd_port     = nullptr;
    GraphPortT*  graph_port  = nullptr;

    ResponseT*   response    = nullptr;
    void*        arg1        = nullptr;
    void*        arg2        = nullptr;
};

typedef Message<0, SoundDriverAudioInput,   SignalSource,  Response_AddAudioInput>   Message_AddAudioInput;
typedef Message<1, SoundDriverAudioOutput,  SignalSink,    Response_AddAudioOutput>  Message_AddAudioOutput;
typedef Message<2, SoundDriverAudioInput,   SignalSource,  Response_RemovePort>      Message_RemoveAudioInput;
typedef Message<3, SoundDriverAudioOutput,  SignalSink,    Response_RemovePort>      Message_RemoveAudioOutput;

#define R64FX_CASE_RECIEVED(A) case Message_##A ::Key() : { recieved((Message_##A*)msg.value());  break; }



/*======= Worker Thread =======*/

struct Impl_SoundDriverAudioInput : public LinkedList<Impl_SoundDriverAudioInput>::Node{
    friend class SoundDriverThreadObjectImpl;

    SignalGraphNode_BufferReader node;
    SoundDriverAudioInput*  sd_port  = nullptr;

    Impl_SoundDriverAudioInput(Message_AddAudioInput* message)
    {
        sd_port = message->sd_port;
        message->graph_port = node.source();
    }

    inline void readSamples(int nsamples)
    {
        sd_port->readSamples(node.buffer(), nsamples);
    }
};

struct Impl_SoundDriverAudioOutput : public LinkedList<Impl_SoundDriverAudioOutput>::Node{
    friend class SoundDriverThreadObjectImpl;

    SignalGraphNode_BufferWriter  node;
    SoundDriverAudioOutput*  sd_port  = nullptr;

    Impl_SoundDriverAudioOutput(Message_AddAudioOutput* message)
    {
        sd_port = message->sd_port;
        message->graph_port = node.sink();
    }

    inline void writeSamples(int nsamples)
    {
        sd_port->writeSamples(node.buffer(), nsamples);
    }
};


class SoundDriverThreadObjectImpl : public ModuleThreadObjectImpl{
    LinkedList<Impl_SoundDriverAudioInput>   m_inputs;
    LinkedList<Impl_SoundDriverAudioOutput>  m_outputs;

public:
    SoundDriverThreadObjectImpl(SoundDriverDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        setPrologue([](void* arg){
            auto self = (SoundDriverThreadObjectImpl*) arg;
            self->prologue();
        }, this);

        setEpilogue([](void* arg){
            auto self = (SoundDriverThreadObjectImpl*) arg;
            self->epilogue();
        }, this);
    }

    ~SoundDriverThreadObjectImpl()
    {
    }

    void storeWithdrawalArgs(SoundDriverWithdrawalAgent* agent)
    {
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
            R64FX_CASE_RECIEVED(AddAudioInput)
            R64FX_CASE_RECIEVED(AddAudioOutput)
            R64FX_CASE_RECIEVED(RemoveAudioInput)
            R64FX_CASE_RECIEVED(RemoveAudioOutput)

            default:
            {
                abort(); //Must never happen!
            }
        }

        sendMessagesToIface(&msg, 1); //Always respond so that iface can free the message payload.
    }

    inline void recieved(Message_AddAudioInput* message)
    {
        auto impl = allocObj<Impl_SoundDriverAudioInput>(message);
        impl->node.resizeBuffer(bufferSize());
//         addGraphElement(&impl->node);
        m_inputs.append(impl);
    }

    inline void recieved(Message_AddAudioOutput* message)
    {
        auto impl = allocObj<Impl_SoundDriverAudioOutput>(message);
        impl->node.resizeBuffer(bufferSize());
//         addGraphElement(&impl->node);
        m_outputs.append(impl);
    }

    inline void recieved(Message_RemoveAudioInput* message)
    {
        Impl_SoundDriverAudioInput* impl = nullptr;
        for(auto obj : m_inputs) if(obj->node.source() == message->graph_port){ impl = obj; break; }
        if(impl)
        {
//             removeGraphElement(&impl->node);
            m_inputs.remove(impl);
            message->sd_port = impl->sd_port;
            freeObj(impl);
        }
    }

    inline void recieved(Message_RemoveAudioOutput* message)
    {
        Impl_SoundDriverAudioOutput* impl = nullptr;
        for(auto obj : m_outputs) if(obj->node.sink() == message->graph_port){ impl = obj; break; }
        if(impl)
        {
//             removeGraphElement(&impl->node);
            m_outputs.remove(impl);
            message->sd_port = impl->sd_port;
            freeObj(impl);
        }
    }

    inline void prologue()
    {
        for(auto input : m_inputs)
        {
            input->readSamples(bufferSize());
        }
    }

    inline void epilogue()
    {
        for(auto output : m_outputs)
        {
            output->writeSamples(bufferSize());
        }
    }
};

R64FX_DEF_MODULE_AGENTS(SoundDriver)



/*======= Main Thread =======*/

class SoundDriverThreadObjectIface : public ModuleThreadObjectIface{
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

public:
    template<typename MessageT, typename ResponseT>
    inline void addAudioPort(const char* name, ResponseT* response, void* arg1, void* arg2)
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd);
        assert(name);
        assert(response);
#endif//R64FX_DEBUG

        auto message = new MessageT(response, arg1, arg2);
        sd->newPort(&message->sd_port, name);
#ifdef R64FX_DEBUG
        assert(message->sd_port);
#endif//R64FX_DEBUG

        ThreadObjectMessage msg(MessageT::Key(), message);
        sendMessagesToImpl(&msg, 1);
    }

    template<typename ModulePortT, typename MessageT>
    inline void removeAudioPort(ModulePortT* port, Response_RemovePort* response, void* arg1, void* arg2)
    {
        auto message = new MessageT(response, arg1, arg2);
        ModulePrivate::getPortPayload(port, message->graph_port);
#ifdef R64FX_DEBUG
        assert(message->graph_port);
#endif//R64FX_DEBUG

        ThreadObjectMessage msg(MessageT::Key(), message);
        sendMessagesToImpl(&msg, 1);
    }

private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        switch(msg.key())
        {
            R64FX_CASE_RECIEVED(AddAudioInput)
            R64FX_CASE_RECIEVED(AddAudioOutput)
            R64FX_CASE_RECIEVED(RemoveAudioInput)
            R64FX_CASE_RECIEVED(RemoveAudioOutput)

            default:
            {
                abort();
            }
        }
    }

    inline void recieved(Message_AddAudioInput* message)
    {
        recievedAddAudioPort(message);
    }

    inline void recieved(Message_AddAudioOutput* message)
    {
        recievedAddAudioPort(message);
    }

    inline void recieved(Message_RemoveAudioInput* message)
    {
        recievedRemoveAudioPort(message);
    }

    inline void recieved(Message_RemoveAudioOutput* message)
    {
        recievedRemoveAudioPort(message);
    }

    template<typename MessageT> inline void recievedAddAudioPort(MessageT* message)
    {
#ifdef R64FX_DEBUG
        assert(message->response);
        assert(message->graph_port);
#endif//R64FX_DEBUG
        auto module_port = newPortFromMessageType(message);
        ModulePrivate::setPortPayload(module_port, message->graph_port);
        message->response(module_port, message->arg1, message->arg2);
        delete message;
    }

    template<typename MessageT> inline void recievedRemoveAudioPort(MessageT* message)
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(message->response);
        assert(message->graph_port);
        assert(message->sd_port);
        assert(sd);
#endif//R64FX_DEBUG
        sd->deletePort(message->sd_port);
        message->response(message->arg1, message->arg2);
        delete message;
    }

    inline ModuleSource* newPortFromMessageType(Message_AddAudioInput*)
    {
        return new ModuleSource;
    }

    inline ModuleSink* newPortFromMessageType(Message_AddAudioOutput*)
    {
        return new ModuleSink;
    }
};

}//namespace

#define m_thread_object_iface ((SoundDriverThreadObjectIface*)m)


Module_SoundDriver::Module_SoundDriver()
{
    m = new(std::nothrow) SoundDriverThreadObjectIface;
}


Module_SoundDriver::~Module_SoundDriver()
{
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG
    delete m_thread_object_iface;
}


bool Module_SoundDriver::engage(ModuleCallback done, void* done_arg)
{
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG
    deploy_tobj(this, m_thread_object_iface, done, done_arg);
    return true;
}


void Module_SoundDriver::disengage(ModuleCallback done, void* done_arg)
{
#ifdef R64FX_DEBUG
    assert(isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG
    withdraw_tobj(this, m_thread_object_iface, done, done_arg);
}


bool Module_SoundDriver::isEngaged()
{
    return m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_SoundDriver::engagementPending()
{
    return m_thread_object_iface->isPending();
}


void Module_SoundDriver::addAudioInput(const char* name, Response_AddAudioInput* response, void* arg1, void* arg2)
{
    m_thread_object_iface->addAudioPort<Message_AddAudioInput, Response_AddAudioInput>(name, response, arg1, arg2);
}


void Module_SoundDriver::addAudioOutput(const char* name, Response_AddAudioOutput* response, void* arg1, void* arg2)
{
    m_thread_object_iface->addAudioPort<Message_AddAudioOutput, Response_AddAudioOutput>(name, response, arg1, arg2);
}


void Module_SoundDriver::removePort(ModulePort* port, Response_RemovePort* response, void* arg1, void* arg2)
{
    if(port->isSource())
    {
        m_thread_object_iface->removeAudioPort<ModuleSource, Message_RemoveAudioInput>((ModuleSource*)port, response, arg1, arg2);
    }
    else
    {
        m_thread_object_iface->removeAudioPort<ModuleSink, Message_RemoveAudioOutput>((ModuleSink*)port, response, arg1, arg2);
    }
}

}//namespace r64fx
