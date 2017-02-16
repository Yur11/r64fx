#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObject.hpp"
#include "ModuleFlags.hpp"

#define R64FX_DEF_MODULE_IMPL_ARGS ModuleThreadObjectImpl* parent_impl, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS
#define R64FX_MODULE_IMPL_ARGS parent_impl, R64FX_THREAD_OBJECT_IMPL_ARGS

namespace r64fx{

class SoundDriver;
class SoundDriverSyncPort;
class ModuleThreadObjectIfaceHandle;
class ModuleThreadObjectImplHandle;

/*
 * === Impl ===================================================================
 */

class ModuleThreadObjectImpl : public ThreadObjectImpl, public LinkedList<ModuleThreadObjectImpl>::Node{
    ModuleThreadObjectImpl*             m_parent    = nullptr;
    LinkedList<ModuleThreadObjectImpl>  m_children;

protected:
    unsigned long m_flags = 0;

public:
    ModuleThreadObjectImpl(R64FX_DEF_MODULE_IMPL_ARGS);

    virtual ~ModuleThreadObjectImpl();

protected:

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

    SoundDriverSyncPort*           sync_port           = nullptr;
    ModuleThreadObjectImplHandle*  parent_impl_handle  = nullptr;

    virtual ThreadObjectImpl* deployImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) override final;

    virtual ModuleThreadObjectImpl* deployModuleImpl(HeapAllocator* ha, R64FX_DEF_MODULE_IMPL_ARGS) = 0;
};


class ModuleWithdrawalAgent : public ThreadObjectWithdrawalAgent{
    ModuleThreadObjectImplHandle*  parent_impl_handle  = nullptr;

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
