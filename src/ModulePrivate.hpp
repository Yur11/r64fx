#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObject.hpp"
#include "ModuleFlags.hpp"


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


#define R64FX_DEF_MODULE_AGENTS(NAME)\
ModuleThreadObjectImpl* NAME##DeploymentAgent::deployModuleImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)\
{\
    return ha->allocObj<NAME##ThreadObjectImpl>(this, R64FX_THREAD_OBJECT_IMPL_ARGS);\
}\
\
void NAME##WithdrawalAgent::withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl)\
{\
    auto osc_impl = static_cast<NAME##ThreadObjectImpl*>(impl);\
    osc_impl->storeWithdrawalArgs(this);\
    ha->freeObj(osc_impl);\
}


namespace r64fx{

class SoundDriver;
class SoundDriverSyncPort;
class ModuleThreadObjectIfaceHandle;
class ModuleThreadObjectImplHandle;
class ModuleDeploymentAgent;

/*
 * === Impl ===================================================================
 */

class ModuleThreadObjectImpl : public ThreadObjectImpl, public LinkedList<ModuleThreadObjectImpl>::Node{
    ModuleThreadObjectImpl*             m_parent    = nullptr;
    LinkedList<ModuleThreadObjectImpl>  m_children;

protected:
    unsigned long m_flags = 0;

public:
    ModuleThreadObjectImpl(ModuleDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS);

    virtual ~ModuleThreadObjectImpl();

    SoundDriverSyncPort* syncPort();

protected:
    void setPrologue(void (*fun)(void* arg), void* arg = nullptr);

    void setEpilogue(void (*fun)(void* arg), void* arg = nullptr);

    long bufferSize();

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) override;

    virtual void runThread() override final;

    virtual void exitThread() override final;
};


/*
 * === Agents =================================================================
 */

class ModuleDeploymentAgent : public ThreadObjectDeploymentAgent{
    friend class ModuleThreadObjectIface;
    friend class ModuleThreadObjectImpl;

    SoundDriverSyncPort*     sync_port    = nullptr; // ->
    ModuleThreadObjectImpl*  parent_impl  = nullptr; // ->
    long                     buffer_size  = 0;
    long                     sample_rate  = 0;

    virtual ThreadObjectImpl* deployImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) override final;

    virtual ModuleThreadObjectImpl* deployModuleImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) = 0;
};


class ModuleWithdrawalAgent : public ThreadObjectWithdrawalAgent{
    friend class ModuleThreadObjectIface;
    friend class ModuleThreadObjectImpl;

    SoundDriverSyncPort*     sync_port    = nullptr; // <-

    virtual void withdrawImpl(HeapAllocator* ha, ThreadObjectImpl* impl) override final;

    virtual void withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl) = 0;
};


/*
 * === Iface ==================================================================
 */

class ModuleThreadObjectIface : public ThreadObjectIface{
protected:
    SoundDriver* soundDriver();

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

}//namespace r64fx

#endif//R64FX_MODULE_PRIVATE_HPP
