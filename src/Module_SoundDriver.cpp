#include "Module_SoundDriver.hpp"
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"
#include "SignalNode_BufferRW.hpp"

namespace r64fx{

namespace{

R64FX_DECL_DEFAULT_MODULE_AGENTS(SoundDriver);

template<unsigned long MsgKey, typename SDPortT, typename NodePortT, typename ResponseT>
struct Message{
    inline constexpr static unsigned long Key() { return MsgKey; }

    Message(ResponseT* response, void* arg1, void* arg2)
    : response(response), arg1(arg1), arg2(arg2) {}

    SDPortT*    sd_port     = nullptr;
    NodePortT*  node_port   = nullptr;

    ResponseT*  response    = nullptr;
    void*       arg1        = nullptr;
    void*       arg2        = nullptr;
};

typedef Message<0, SoundDriverAudioInput,   SignalSource,  Callback_AddAudioInput>   Message_AddAudioInput;
typedef Message<1, SoundDriverAudioOutput,  SignalSink,    Callback_AddAudioOutput>  Message_AddAudioOutput;
typedef Message<2, SoundDriverAudioInput,   SignalSource,  Callback_RemovePort>      Message_RemoveAudioInput;
typedef Message<3, SoundDriverAudioOutput,  SignalSink,    Callback_RemovePort>      Message_RemoveAudioOutput;

#define R64FX_CASE_RECIEVED(A) case Message_##A ::Key() : { recieved((Message_##A*)msg.value());  break; }



/*======= Worker Thread =======*/


/* A pair of reader and writer noded can share a single buffer. */
struct SoundDriverPortsImpl : public LinkedList<SoundDriverPortsImpl>::Node{
    SoundDriverAudioInput*    input   = nullptr;
    SoundDriverAudioOutput*   output  = nullptr;
    SignalNode_BufferReader*  reader  = nullptr;
    SignalNode_BufferWriter*  writer  = nullptr;

/* For access to members from template functions. */
#define R64FX_ACCESS_METHODS(AddOrRemove, Name, Member1, Member2)\
    inline auto  Name           (Message_##AddOrRemove##AudioInput*)  ->decltype(Member1)& { return Member1; }\
    inline auto  Name           (Message_##AddOrRemove##AudioOutput*) ->decltype(Member2)& { return Member2; }\
    inline auto  second_##Name  (Message_##AddOrRemove##AudioInput*)  ->decltype(Member2)& { return Member2; }\
    inline auto  second_##Name  (Message_##AddOrRemove##AudioOutput*) ->decltype(Member1)& { return Member1; }
    R64FX_ACCESS_METHODS(Add,     port,  input,  output)
    R64FX_ACCESS_METHODS(Remove,  port,  input,  output)
    R64FX_ACCESS_METHODS(Add,     node,  reader, writer)
    R64FX_ACCESS_METHODS(Remove,  node,  reader, writer)
#undef R64FX_ACCESS_METHODS

#define R64FX_ACCESS_METHODS(AddOrRemove)\
    inline SignalSource* node_port(Message_##AddOrRemove##AudioInput*) { return reader->out(); }\
    inline SignalSink*   node_port(Message_##AddOrRemove##AudioOutput*) { return writer->in(); }
    R64FX_ACCESS_METHODS(Add)
    R64FX_ACCESS_METHODS(Remove)
#undef R64FX_ACCESS_METHODS

};


class SoundDriverThreadObjectImpl : public ModuleThreadObjectImpl{
    LinkedList<SoundDriverPortsImpl> m_impls;

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

    /* Using message type with oveloaded methods in SoundDriverPortsImpl. */
    template<typename MsgT, typename NodeT> inline void addAudioPort(MsgT* message)
    {
        R64FX_DEBUG_ASSERT(message->sd_port);

        auto sg = signalGraph();

        DataBufferPointer buffer;
        auto impl = m_impls.first();
        while(impl && impl->port(message)) impl = impl->next();
        if(impl)
        {
            R64FX_DEBUG_ASSERT(impl->output);
            R64FX_DEBUG_ASSERT(impl->writer);
            buffer = impl->second_node(message)->buffer();
        }
        else
        {
            impl = allocObj<SoundDriverPortsImpl>();
            m_impls.append(impl);
            R64FX_DEBUG_ASSERT(signalGraph());
            buffer = signalGraph()->allocBuffer();
        }

        impl->port(message) = message->sd_port;
        impl->node(message) = allocObj<NodeT, SignalGraph&, DataBufferPointer>(*sg, buffer);
        message->node_port = impl->node_port(message);
    }

    inline void recieved(Message_AddAudioInput* message)
    {
        addAudioPort<Message_AddAudioInput, SignalNode_BufferReader>(message);
    }

    inline void recieved(Message_AddAudioOutput* message)
    {
        addAudioPort<Message_AddAudioOutput, SignalNode_BufferWriter>(message);
    }

    template<typename MsgT> void removeAudioPort(MsgT* message)
    {
        R64FX_DEBUG_ASSERT(message->node_port);

        auto impl = m_impls.first();
        while(impl && impl->node_port(message) != message->node_port) impl = impl->next();
        R64FX_DEBUG_ASSERT(impl);
        auto buffer = impl->node(message)->buffer();
        message->sd_port = impl->port(message);
        freeObj(impl->node(message));
        while(impl->next())
        {
            auto next_node = impl->next()->node(message);
            impl->node(message) = next_node;
            auto next_buffer = next_node->buffer();
            next_node->setBuffer(buffer);
            buffer = next_buffer;
            impl = impl->next();
        }
        if(!impl->second_node(message))
        {
            signalGraph()->freeBuffer(buffer);
            m_impls.remove(impl);
            freeObj(impl);
        }
    }

    inline void recieved(Message_RemoveAudioInput* message)
    {
        removeAudioPort(message);
    }

    inline void recieved(Message_RemoveAudioOutput* message)
    {
        removeAudioPort(message);
    }

    inline void prologue()
    {
        auto impl = m_impls.first();
        while(impl && impl->input)
        {
            auto count = impl->input->readSamples(impl->reader->bufferAddr(), signalGraph()->frameCount());
            R64FX_DEBUG_ASSERT(count == int(signalGraph()->frameCount()));
            impl = impl->next();
        }
    }

    inline void epilogue()
    {
        auto impl = m_impls.first();
        while(impl && impl->output)
        {
            auto count = impl->output->writeSamples(impl->writer->bufferAddr(), signalGraph()->frameCount());
            R64FX_DEBUG_ASSERT(count == int(signalGraph()->frameCount()));
            impl = impl->next();
        }
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
        ModulePrivate::getPortPayload(port, message->node_port);
#ifdef R64FX_DEBUG
        assert(message->node_port);
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
        assert(message->node_port);
#endif//R64FX_DEBUG
        auto module_port = newPortFromMessageType(message);
        ModulePrivate::setPortPayload(module_port, message->node_port);
        message->response(module_port, message->arg1, message->arg2);
        delete message;
    }

    template<typename MessageT> inline void recievedRemoveAudioPort(MessageT* message)
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(message->response);
        assert(message->node_port);
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
