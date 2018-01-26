/* To be included in Module.cpp */

#define R64FX_SAME_TYPE(A, B) (std::is_same<A, B>::value)

namespace r64fx{

namespace{

template<typename ModulePortT_, typename SDPortT_, typename SGNodeT_> struct PortImplRec{
    typedef ModulePortT_  ModulePortT;
    typedef SDPortT_      SDPortT;
    typedef SGNodeT_      SGNodeT;

    /* SoundDriver port. Created & destroyed in main thread. Used in worker thread. */
    SDPortT* sd_port[2] = {nullptr, nullptr};

    /* SignalGraph node. Created, destroyed & used in worker thread. */
    SGNodeT* sg_node = nullptr;
};


/* We have 2 valid Module_SoundDriver port types. */
template<typename T> struct PortImpl{};

template<> struct PortImpl<ModuleSoundDriverInputSource>
: public ModuleSignalSourceImpl, public PortImplRec<ModuleSoundDriverInputSource, SoundDriverAudioInput, SignalNode_BufferReader>{
    inline void setNode(SGNodeT* node) { sg_node = node; signal_port = node->out(); }

    inline ModuleSoundDriverInputSource* handle() { return (ModuleSoundDriverInputSource*)this; }

    inline static PortImpl<ModuleSoundDriverInputSource>* From(ModuleSoundDriverInputSource* handle)
        { return (PortImpl<ModuleSoundDriverInputSource>*)handle; }
};

template<> struct PortImpl<ModuleSoundDriverOutputSink>
: public ModuleSignalSinkImpl, public PortImplRec<ModuleSoundDriverOutputSink, SoundDriverAudioOutput, SignalNode_BufferWriter>{
    inline void setNode(SGNodeT* node) { sg_node = node; signal_port = node->in(); }

    inline ModuleSoundDriverOutputSink* handle() { return (ModuleSoundDriverOutputSink*)this; }

    inline static PortImpl<ModuleSoundDriverOutputSink>* From(ModuleSoundDriverOutputSink* handle)
        { return (PortImpl<ModuleSoundDriverOutputSink>*)handle; }
};


template<unsigned long MsgKey, typename ModulePortT_, typename CallbackT>
struct Message{
    inline constexpr static unsigned long Key() { return MsgKey; }

    typedef          ModulePortT_           ModulePortT;
    typedef          PortImpl<ModulePortT>  PortImplT;
    typedef typename PortImplT::SGNodeT     SGNodeT;

    PortImplT* port_impl = nullptr;

    CallbackT*     callback    = nullptr;
    void*          arg1        = nullptr;
    void*          arg2        = nullptr;

    Message(CallbackT* callback, void* arg1, void* arg2)
    : callback(callback), arg1(arg1), arg2(arg2) {}

    inline static PortImplT* ImplFrom(ModulePortT* handle)
        { return (PortImplT*)handle; }
};

typedef Message<0, ModuleSoundDriverInputSource, Callback_AddAudioInput>      Message_AddAudioInput;
typedef Message<1, ModuleSoundDriverOutputSink,  Callback_AddAudioOutput>     Message_AddAudioOutput;
typedef Message<2, ModuleSoundDriverInputSource, Callback_RemoveAudioInput>   Message_RemoveAudioInput;
typedef Message<3, ModuleSoundDriverOutputSink,  Callback_RemoveAudioOutput>  Message_RemoveAudioOutput;

#define R64FX_CASE_RECIEVED(M) case Message_##M::Key(): { recieved((Message_##M*)msg.value()); break; }

/*
 * === Worker Thread ===================================================================
 */

R64FX_DECL_DEFAULT_MODULE_AGENTS(SoundDriver);

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
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) override final
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
    }

    inline void recieved(Message_AddAudioInput*      msg) { addAudioPort    (msg); }
    inline void recieved(Message_AddAudioOutput*     msg) { addAudioPort    (msg); }
    inline void recieved(Message_RemoveAudioInput*   msg) { removeAudioPort (msg); }
    inline void recieved(Message_RemoveAudioOutput*  msg) { removeAudioPort (msg); }

    template<typename MsgT> inline void addAudioPort(MsgT* msg)
    {
        auto pi = msg->port_impl;
        R64FX_DEBUG_ASSERT(pi);

        R64FX_DEBUG_ASSERT(pi->sd_port[0]);
        float* buff0 = alloc_buffer<float>(bufferSize());
        soundDriverPortGroup()->updatePort(pi->sd_port[0], buff0);

        float* buff1 = nullptr;
        if(pi->sd_port[1])
        {
            buff1 = alloc_buffer<float>(bufferSize());
            soundDriverPortGroup()->updatePort(pi->sd_port[1], buff1);
        }

        auto node = allocObj<typename MsgT::SGNodeT>(signalGraph(), buff0, buff1);
        pi->setNode(node);
        if constexpr(std::is_same<typename MsgT::SGNodeT, SignalNode_BufferWriter>::value)
        {
            addGraphOutput(node);
        }
        ThreadObjectMessage response(MsgT::Key(), msg);
        sendMessagesToIface(&response, 1);
    }

    template<typename MsgT> void removeAudioPort(MsgT* msg)
    {
        auto pi = msg->port_impl;
        R64FX_DEBUG_ASSERT(pi);

        R64FX_DEBUG_ASSERT(pi->sg_node);
        R64FX_DEBUG_ASSERT(pi->sg_node->buffer(0));
        R64FX_DEBUG_ASSERT(pi->sd_port[0]);

        if(pi->sg_node->buffer(1))
        {
            soundDriverPortGroup()->updatePort(pi->sd_port[1], nullptr);
            free(pi->sg_node->buffer(1));
        }

        /* Make sure that callback is issued only when all other ports have been updated. */
        soundDriverPortGroup()->updatePort(pi->sd_port[0], nullptr,
        [](SoundDriverAudioPort* port, float* buffer, void* arg0, void* arg1){
            ((SoundDriverThreadObjectImpl*)arg0)->portBufferRemoved<MsgT>((MsgT*)arg1, port, buffer);
        }, this, msg);
        free(pi->sg_node->buffer(0));

        if constexpr(R64FX_SAME_TYPE(typename MsgT::SGNodeT, SignalNode_BufferWriter))
        {
            removeGraphOutput(pi->sg_node);
        }
        freeObj(pi->sg_node);
        pi->sg_node = nullptr;
        pi->signal_port = nullptr;

        armRebuild();
    }

    template<typename MsgT> inline void portBufferRemoved(MsgT* msg, SoundDriverAudioPort* port, float* buffer)
    {
        ThreadObjectMessage response(MsgT::Key(), msg);
        sendMessagesToIface(&response, 1);
    }
};

R64FX_DEF_MODULE_AGENTS(SoundDriver)



/*======= Main Thread =======*/

class SoundDriverThreadObjectIface : public ModuleThreadObjectIface{
    R64FX_USE_EMPTY_MODULE_AGENTS(SoundDriver)

    /* Recieving Replies from Worker Thread */

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

    inline void recieved(Message_AddAudioInput*     msg)  { recievedAddAudioPort    (msg); }
    inline void recieved(Message_AddAudioOutput*    msg)  { recievedAddAudioPort    (msg); }
    inline void recieved(Message_RemoveAudioInput*  msg)  { recievedRemoveAudioPort (msg); }
    inline void recieved(Message_RemoveAudioOutput* msg)  { recievedRemoveAudioPort (msg); }

    template<typename MsgT> inline void recievedAddAudioPort(MsgT* msg)
    {
        auto pi = msg->port_impl;
        R64FX_DEBUG_ASSERT(pi);

        R64FX_DEBUG_ASSERT(msg->callback);
        msg->callback(pi->handle(), msg->arg1, msg->arg2);

        delete msg;
    }

    template<typename MsgT> inline void recievedRemoveAudioPort(MsgT* msg)
    {
        auto sd = soundDriver();
        R64FX_DEBUG_ASSERT(sd);

        auto pi = msg->port_impl;
        R64FX_DEBUG_ASSERT(pi);

        sd->deletePort(pi->sd_port[0]);
        if(pi->sd_port[1])
            sd->deletePort(pi->sd_port[1]);
        delete pi;

        R64FX_DEBUG_ASSERT(msg->callback);
        msg->callback(msg->arg1, msg->arg2);

        delete msg;
    }

public:
    /* Sending Messages to Worker Thread */

    template<typename MsgT, typename CallbackT>
    inline void addAudioPort(const std::string &name, unsigned int chan_count, CallbackT* callback, void* arg1, void* arg2)
    {
        R64FX_DEBUG_ASSERT(!name.empty());
        R64FX_DEBUG_ASSERT(chan_count == 1 || chan_count == 2);

        auto sd = soundDriver();
        R64FX_DEBUG_ASSERT(sd);
        R64FX_DEBUG_ASSERT(callback);

        auto msg = new MsgT(callback, arg1, arg2);
        auto pi = msg->port_impl = new typename MsgT::PortImplT;
        if(chan_count == 1)
        {
            sd->newPort(pi->sd_port, name);
        }
        else
        {
            sd->newPort(pi->sd_port,     name + "1");
            sd->newPort(pi->sd_port + 1, name + "2");
        }
        pi->thread = thread();
        std::cout << "thread: " << pi->thread << "\n";

        ThreadObjectMessage msgs(MsgT::Key(), msg);
        sendMessagesToImpl(&msgs, 1);
    }

    template<typename MsgT, typename CallbackT>
    inline void removeAudioPort(typename MsgT::ModulePortT* port, CallbackT* callback, void* arg1, void* arg2)
    {
        auto msg = new MsgT(callback, arg1, arg2);
        msg->port_impl = MsgT::ImplFrom(port);
        ThreadObjectMessage msgs(MsgT::Key(), msg);
        sendMessagesToImpl(&msgs, 1);
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
    R64FX_DEBUG_ASSERT(!isEngaged());
    R64FX_DEBUG_ASSERT(!engagementPending());
    delete m_thread_object_iface;
}


bool Module_SoundDriver::engage(Module::Callback* done, void* done_arg, ModuleThreadHandle* threads, int nthreads)
{
    R64FX_DEBUG_ASSERT(!isEngaged());
    R64FX_DEBUG_ASSERT(!engagementPending());
    ModulePrivate::deploy(m_thread_object_iface, nullptr, done, done_arg, this);
    return true;
}


void Module_SoundDriver::disengage(Module::Callback* done, void* done_arg)
{
    R64FX_DEBUG_ASSERT(isEngaged());
    R64FX_DEBUG_ASSERT(!engagementPending());
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


void Module_SoundDriver::addAudioInput(
    const char* name, unsigned int chan_count, Callback_AddAudioInput* callback, void* arg1, void* arg2
)
{
    m_thread_object_iface->addAudioPort<Message_AddAudioInput>(
        name, chan_count, callback, arg1, arg2
    );
}


void Module_SoundDriver::addAudioOutput(
    const char* name, unsigned int chan_count, Callback_AddAudioOutput* callback, void* arg1, void* arg2
)
{
    m_thread_object_iface->addAudioPort<Message_AddAudioOutput>(
        name, chan_count, callback, arg1, arg2
    );
}


void Module_SoundDriver::removePort(
    ModuleSoundDriverInputSource* source, Callback_RemoveAudioInput* callback, void* arg1, void* arg2
)
{
    m_thread_object_iface->removeAudioPort<Message_RemoveAudioInput>(
        source, callback, arg1, arg2
    );
}


void Module_SoundDriver::removePort(
    ModuleSoundDriverOutputSink* sink,  Callback_RemoveAudioOutput* callback, void* arg1, void* arg2
)
{
    m_thread_object_iface->removeAudioPort<Message_RemoveAudioOutput>(
        sink, callback, arg1, arg2
    );
}

}//namespace r64fx
