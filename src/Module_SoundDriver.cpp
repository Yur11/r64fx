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

typedef Message<0, SoundDriverAudioInput,   SignalSource,  Callback_AddAudioInput>   Message_AddAudioInput;
typedef Message<1, SoundDriverAudioOutput,  SignalSink,    Callback_AddAudioOutput>  Message_AddAudioOutput;
typedef Message<2, SoundDriverAudioInput,   SignalSource,  Callback_RemovePort>      Message_RemoveAudioInput;
typedef Message<3, SoundDriverAudioOutput,  SignalSink,    Callback_RemovePort>      Message_RemoveAudioOutput;

#define R64FX_CASE_RECIEVED(A) case Message_##A ::Key() : { recieved((Message_##A*)msg.value());  break; }



/*======= Worker Thread =======*/

// struct Impl_SoundDriverAudioInput : public LinkedList<Impl_SoundDriverAudioInput>::Node{
//     friend class SoundDriverThreadObjectImpl;
// 
//     SignalNode_BufferReader node;
//     SoundDriverAudioInput*  sd_port  = nullptr;
// 
//     Impl_SoundDriverAudioInput(Message_AddAudioInput* message, SignalGraph* sg)
//     {
//         sd_port = message->sd_port;
//         message->graph_port = node.out().port();
// //         sg->addNode(&node);
//     }
// 
//     inline void readSamples(int nsamples)
//     {
//         sd_port->readSamples(node.buffer(), nsamples);
//     }
// };
// 
// struct Impl_SoundDriverAudioOutput : public LinkedList<Impl_SoundDriverAudioOutput>::Node{
//     friend class SoundDriverThreadObjectImpl;
// 
//     SignalNode_BufferWriter  node;
//     SoundDriverAudioOutput*  sd_port  = nullptr;
// 
//     Impl_SoundDriverAudioOutput(Message_AddAudioOutput* message, SignalGraph* sg)
//     {
//         sd_port = message->sd_port;
//         message->graph_port = node.in().port();
// //         sg->addNode(&node);
//     }
// 
//     inline void writeSamples(int nsamples)
//     {
//         sd_port->writeSamples(node.buffer(), nsamples);
//     }
// };


struct SoundDriverPortsImpl : public LinkedList<SoundDriverPortsImpl>{
    SoundDriverAudioInput*    input   = nullptr;
    SoundDriverAudioOutput*   output  = nullptr;
    SignalNode_BufferReader*  reader  = nullptr;
    SignalNode_BufferWriter*  writer  = nullptr;
};


class SoundDriverThreadObjectImpl : public ModuleThreadObjectImpl{
    LinkedList<SoundDriverPortsImpl> m_ports;

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
                abort(); //Must never happen!
        }

        sendMessagesToIface(&msg, 1); //Always respond so that iface can free the message payload.
    }

    inline void recieved(Message_AddAudioInput* message)
    {

    }

    inline void recieved(Message_AddAudioOutput* message)
    {

    }

    inline void recieved(Message_RemoveAudioInput* message)
    {

    }

    inline void recieved(Message_RemoveAudioOutput* message)
    {

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
    R64FX_USE_EMPTY_MODULE_AGENTS(SoundDriver)

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
    inline void removeAudioPort(ModulePortT* port, Callback_RemovePort* response, void* arg1, void* arg2)
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
                abort();
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


bool Module_SoundDriver::engage(Module::Callback* done, void* done_arg, ModuleThreadHandle* threads, int nthreads)
{
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG
    ModulePrivate::deploy(m_thread_object_iface, nullptr, done, done_arg, this);
    return true;
}


void Module_SoundDriver::disengage(Module::Callback* done, void* done_arg)
{
#ifdef R64FX_DEBUG
    assert(isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG
    ModulePrivate::withdraw(m_thread_object_iface, done, done_arg, this);
}


bool Module_SoundDriver::isEngaged()
{
    return m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_SoundDriver::engagementPending()
{
    return m_thread_object_iface->isPending();
}


void Module_SoundDriver::addAudioInput(const char* name, Callback_AddAudioInput* response, void* arg1, void* arg2)
{
    m_thread_object_iface->addAudioPort<Message_AddAudioInput, Callback_AddAudioInput>(name, response, arg1, arg2);
}


void Module_SoundDriver::addAudioOutput(const char* name, Callback_AddAudioOutput* response, void* arg1, void* arg2)
{
    m_thread_object_iface->addAudioPort<Message_AddAudioOutput, Callback_AddAudioOutput>(name, response, arg1, arg2);
}


void Module_SoundDriver::removePort(ModulePort* port, Callback_RemovePort* response, void* arg1, void* arg2)
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
