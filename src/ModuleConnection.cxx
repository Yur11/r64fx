/* To be included in Moduel.cpp */

namespace r64fx{

namespace{

R64FX_DECL_DEFAULT_MODULE_AGENTS(ModuleConnectionManager);

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
        switch(msg.key())
        {
            default:
            {
                abort(); //Must never happen!
            }
        }

        sendMessagesToIface(&msg, 1); //Always respond so that iface can free the message payload.
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
        switch(msg.key())
        {
            default:
            {
                abort();
            }
        }
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
