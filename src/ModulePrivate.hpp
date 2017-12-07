#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObject.hpp"
#include "SignalGraph.hpp"


#define R64FX_DECL_MODULE_AGENTS(NAME)\
class NAME##DeploymentAgent : public ModuleDeploymentAgent, public NAME##DeploymentArgs{\
    friend class NAME##ThreadObjectImpl;\
    friend class NAME##ThreadObjectIface;\
    virtual ModuleThreadObjectImpl* deployModuleImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) override final;\
};\
\
class NAME##WithdrawalAgent : public ModuleWithdrawalAgent, public NAME##WithdrawalArgs{\
    friend class NAME##ThreadObjectImpl;\
    friend class NAME##ThreadObjectIface;\
    virtual void withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl) override final;\
};


#define R64FX_DECL_DEFAULT_MODULE_AGENTS(NAME)\
class NAME##DeploymentAgent : public ModuleDeploymentAgent{\
    virtual ModuleThreadObjectImpl* deployModuleImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) override final;\
};\
\
class NAME##WithdrawalAgent : public ModuleWithdrawalAgent{\
    virtual void withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl) override final;\
};


#define R64FX_DEF_MODULE_AGENTS(NAME)\
ModuleThreadObjectImpl* NAME##DeploymentAgent::deployModuleImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)\
{\
    return ha->allocObj<NAME##ThreadObjectImpl>(this, R64FX_THREAD_OBJECT_IMPL_ARGS);\
}\
\
void NAME##WithdrawalAgent::withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl)\
{\
    auto NAME##_impl = static_cast<NAME##ThreadObjectImpl*>(impl);\
    NAME##_impl->storeWithdrawalArgs(this);\
    NAME##_impl->clearHooks();\
    ha->freeObj(NAME##_impl);\
}


#define R64FX_USE_EMPTY_MODULE_AGENTS(NAME)\
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final\
    {\
        auto agent = new NAME##DeploymentAgent;\
        return agent;\
    }\
\
    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final\
    {\
        delete agent;\
    }\
\
    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final\
    {\
        return new NAME##WithdrawalAgent;\
    }\
\
    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final\
    {\
        auto withdrawal_agent = static_cast<NAME##WithdrawalAgent*>(agent);\
        delete withdrawal_agent;\
    }\


namespace r64fx{

class ModuleThreadObjectIfaceHandle;
class ModuleThreadObjectImplHandle;
class ModuleDeploymentAgent;
class SoundDriver;
class SoundDriverSyncPort;

/*
 * === Impl ===================================================================
 */

class ModuleThreadObjectImpl : public ThreadObjectImpl, public LinkedList<ModuleThreadObjectImpl>::Node{

public:
    ModuleThreadObjectImpl(ModuleDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS);

    void clearHooks();

    virtual ~ModuleThreadObjectImpl();

protected:
    SoundDriverSyncPort* syncPort() const;

    long bufferSize() const;

    long sampleRate() const;

    SignalGraph* signalGraph() const;

    void armRebuild();

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) override;

    virtual void runThread() override;

    virtual void exitThread() override;
};


/*
 * === Agents =================================================================
 */

class ModuleDeploymentAgent : public ThreadObjectDeploymentAgent{
    friend class ModuleThreadObjectIface;
    friend class ModuleThreadObjectImpl;

    virtual ThreadObjectImpl* deployImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) override final;

    virtual ModuleThreadObjectImpl* deployModuleImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) = 0;
};


class ModuleWithdrawalAgent : public ThreadObjectWithdrawalAgent{
    friend class ModuleThreadObjectIface;
    friend class ModuleThreadObjectImpl;

    virtual void withdrawImpl(HeapAllocator* ha, ThreadObjectImpl* impl) override final;

    virtual void withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl) = 0;
};


/*
 * === Iface ==================================================================
 */

class ModuleThreadObjectIface : public ThreadObjectIface{
protected:
    SoundDriver* soundDriver();

    ModuleThreadHandle* thread() const;

private:
    inline ModuleThreadObjectImplHandle* impl() const { return (ModuleThreadObjectImplHandle*) ThreadObjectIface::impl(); }

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent() override final;

    virtual ModuleDeploymentAgent* newModuleDeploymentAgent() = 0;


    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent) override final;

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) = 0;


    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent() override final;

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() = 0;


    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent) override final;

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) = 0;
};


class ModulePrivate{
public:
    static void deploy(ModuleThreadObjectIface* iface, ModuleThreadHandle* thread, Module::Callback* done, void* done_arg, Module* module);

    static void withdraw(ModuleThreadObjectIface* iface, Module::Callback* done, void* done_arg, Module* module);

    inline static void setPortPayload(ModuleSink* sink, SignalSink* payload)
    {
        sink->m_payload = (void*) payload;
    }

    inline static void setPortPayload(ModuleSource* source, SignalSource* payload)
    {
        source->m_payload = (void*) payload;
    }

    inline static void getPortPayload(ModuleSink* sink, SignalSink* &payload)
    {
        payload = (SignalSink*) sink->m_payload;
    }

    inline static void getPortPayload(ModuleSource* source, SignalSource* &payload)
    {
        payload = (SignalSource*) source->m_payload;
    }

    inline static void setPortThread(ModulePort* port, ModuleThreadHandle* handle)
    {
        port->m_thread_handle = handle;
    }

    inline static void getPortThread(ModulePort* port, ModuleThreadHandle* &handle)
    {
        handle = port->m_thread_handle;
    }
};

}//namespace r64fx

#endif//R64FX_MODULE_PRIVATE_HPP
