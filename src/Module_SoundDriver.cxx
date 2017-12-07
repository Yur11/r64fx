/* To be included in Module.cpp */

namespace r64fx{

namespace{

/*
 * === Agents & Messages ==============================================================
 */

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


/*
 * === Worker Thread ===================================================================
 */

#define R64FX_ACCESS_METHODS(AddOrRemove, Name, Member1, Member2)\
    inline auto  Name           (SoundDriverPortsImpl* impl, Message_##AddOrRemove##AudioInput*)  ->decltype(impl->Member1)& { return impl->Member1; }\
    inline auto  Name           (SoundDriverPortsImpl* impl, Message_##AddOrRemove##AudioOutput*) ->decltype(impl->Member2)& { return impl->Member2; }\
    inline auto  second_##Name  (SoundDriverPortsImpl* impl, Message_##AddOrRemove##AudioInput*)  ->decltype(impl->Member2)& { return impl->Member2; }\
    inline auto  second_##Name  (SoundDriverPortsImpl* impl, Message_##AddOrRemove##AudioOutput*) ->decltype(impl->Member1)& { return impl->Member1; }
    R64FX_ACCESS_METHODS(Add,     port,  input,  output)
    R64FX_ACCESS_METHODS(Remove,  port,  input,  output)
    R64FX_ACCESS_METHODS(Add,     node,  reader, writer)
    R64FX_ACCESS_METHODS(Remove,  node,  reader, writer)
#undef R64FX_ACCESS_METHODS

#define R64FX_ACCESS_METHODS(AddOrRemove)\
    inline SignalSource* node_port(SoundDriverPortsImpl* impl, Message_##AddOrRemove##AudioInput*) { return impl->reader->out(); }\
    inline SignalSink*   node_port(SoundDriverPortsImpl* impl, Message_##AddOrRemove##AudioOutput*) { return impl->writer->in(); }
    R64FX_ACCESS_METHODS(Add)
    R64FX_ACCESS_METHODS(Remove)
#undef R64FX_ACCESS_METHODS


class SoundDriverThreadObjectImpl : public ModuleSoundDriverThreadObjectImpl{

public:
    SoundDriverThreadObjectImpl(SoundDriverDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleSoundDriverThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
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
        auto impl = ports().first();
        while(impl && port(impl, message)) impl = impl->next();
        if(impl)
        {
            R64FX_DEBUG_ASSERT(second_port(impl, message));
            R64FX_DEBUG_ASSERT(second_node(impl, message));
            buffer = second_node(impl, message)->buffer();
        }
        else
        {
            impl = allocObj<SoundDriverPortsImpl>();
            ports().append(impl);
            R64FX_DEBUG_ASSERT(signalGraph());
            buffer = signalGraph()->allocBuffer();
        }

        port(impl, message) = message->sd_port;
        node(impl, message) = allocObj<NodeT, SignalGraph&, DataBufferPointer>(*sg, buffer);
        message->node_port = node_port(impl, message);
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

        auto impl = ports().first();
        while(impl && node_port(impl, message) != message->node_port) impl = impl->next();
        R64FX_DEBUG_ASSERT(impl);
        auto buffer = node(impl, message)->buffer();
        message->sd_port = port(impl, message);
        freeObj(node(impl, message));
        node(impl, message) = nullptr;
        while(impl->next())
        {
            auto next_node = node(impl->next(), message);
            node(impl, message) = next_node;
            auto next_buffer = next_node->buffer();
            next_node->setBuffer(buffer);
            buffer = next_buffer;
            impl = impl->next();
        }
        if(!second_node(impl, message))
        {
            signalGraph()->freeBuffer(buffer);
            ports().remove(impl);
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
        auto impl = ports().first();
        while(impl && impl->input)
        {
            impl->input->readSamples(impl->reader->bufferAddr(), signalGraph()->frameCount());
            impl = impl->next();
        }
    }

    inline void epilogue()
    {
        auto impl = ports().first();
        while(impl && impl->output)
        {
            impl->output->writeSamples(impl->writer->bufferAddr(), signalGraph()->frameCount());
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
        ModulePrivate::setPortThread(module_port, thread());
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
