/* To be included in Moduel.cpp */

namespace r64fx{

namespace{

R64FX_DECL_DEFAULT_MODULE_AGENTS(ModuleConnectionManager);


struct ModuleConnectionMessage{
    SignalSource* source         = nullptr;
    SignalSink*   sink           = nullptr;
    int           source_offset  = 0;
    int           sink_offset    = 0;
    int           size           = 0;
    int           type           = 0;
    void*         handle         = 0;
};


/*======= Worker Thread =======*/

class ModuleConnectionManagerThreadObjectImpl : public ModuleThreadObjectImpl{
public:
    ModuleConnectionManagerThreadObjectImpl(ModuleConnectionManagerDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {

    }

    ~ModuleConnectionManagerThreadObjectImpl()
    {
    }

    void storeWithdrawalArgs(ModuleConnectionManagerWithdrawalAgent* agent)
    {
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        auto* msgs = (ModuleConnectionMessage*) msg.value();
        long nmsgs = msg.key();
#ifdef R64FX_DEBUG
        assert(msgs);
        assert(nmsgs > 0);
#endif//R64FX_DEBUG

        for(int i=0; i<nmsgs; i++)
        {
            auto &message = msgs[i];
#ifdef R64FX_DEBUG
            assert(message.source);
            assert(message.sink);
#endif//R64FX_DEBUG
        }

        sendMessagesToIface(&msg, 1);
    }

    inline void prologue()
    {

    }

    inline void epilogue()
    {

    }
};

R64FX_DEF_MODULE_AGENTS(ModuleConnectionManager)



/*======= Main Thread =======*/

class ModuleConnectionManagerThreadObjectIface : public ModuleThreadObjectIface{
    R64FX_USE_EMPTY_MODULE_AGENTS(ModuleConnectionManager)

public:

private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        auto msgs = (ModuleConnectionMessage*) msg.value();
        long nmsgs = msg.key();
#ifdef R64FX_DEBUG
        assert(msgs);
        assert(nmsgs > 0);
#endif//R64FX_DEBUG
    }
};


}//namespace


ModuleConnection::ModuleConnection(ModuleSource* source_port, ModuleSink* sink)
{

}


bool ModuleConnection::enabled()
{
    return false;
}


void ModuleConnection::enableBulk(ModuleConnection* connections, int nconnections, ModuleConnection::Callback* callback, void* arg)
{
    
}


void ModuleConnection::disableBulk(ModuleConnection* connections, int nconnections, ModuleConnection::Callback* callback, void* arg)
{
    
}

}//namespace r64fx
