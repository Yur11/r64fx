#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
#endif//R64FX_DEBUG

namespace r64fx{

namespace{
    SoundDriver* g_SoundDriver = nullptr;

    struct ModuleImplSharedAssets{
        
    };
}//namespace


/*
 * === Impl ===================================================================
 */

ModuleThreadObjectImpl::ModuleThreadObjectImpl(ThreadObjectIfaceHandle* iface_handle, ThreadObjectManagerImpl* manager_impl)
: ThreadObjectImpl(iface_handle, manager_impl)
{
//     if(asset() == nullptr)
//     {
//         asset = allocObj<ModuleThreadAssets>();
//     }
}


ModuleThreadObjectImpl::~ModuleThreadObjectImpl()
{
//     auto assets = threadAssets();
// #ifdef R64FX_DEBUG
//     assert(assets != nullptr);
// #endif//R64FX_DEBUG
//     assets->count--;
//     if(assets->count == 0)
//     {
//         delete assets;
//         setPayload(nullptr);
//     }
}


// ModuleThreadAssets* ModuleThreadObjectImpl::threadAssets() const
// {
//     return (ModuleThreadAssets*) payload();
// }


void ModuleThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{
    
}


void ModuleThreadObjectImpl::runThread()
{
    m_flags |= R64FX_MODULE_IMPL_THREAD_RUNNING;
    while(m_flags & R64FX_MODULE_IMPL_THREAD_RUNNING)
    {
        readMessagesFromIface();
        //Do Stuff!!
        sleep_nanoseconds(5000 * 1000);
    }
}


void ModuleThreadObjectImpl::exitThread()
{
    m_flags &= ~R64FX_MODULE_IMPL_THREAD_RUNNING;
}


/*
 * === Agents =================================================================
 */

ThreadObjectImpl* ModuleDeploymentAgent::deployImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
{
    auto impl = deployModuleImpl(ha, (ModuleThreadObjectImpl*)parent_impl_handle, R64FX_THREAD_OBJECT_IMPL_ARGS);
    return impl;
}


void ModuleWithdrawalAgent::withdrawImpl(HeapAllocator* ha, ThreadObjectImpl* impl)
{
    withdrawModuleImpl(ha, static_cast<ModuleThreadObjectImpl*>(impl), (ModuleThreadObjectImpl*)parent_impl_handle);
}

/*
 * === Iface ==================================================================
 */

SoundDriver* ModuleThreadObjectIface::soundDriver()
{
    if(!g_SoundDriver)
    {
        g_SoundDriver = SoundDriver::newInstance(SoundDriver::Type::Jack);
    }
    return g_SoundDriver;
}


ThreadObjectDeploymentAgent* ModuleThreadObjectIface::newDeploymentAgent()
{
    auto agent = newModuleDeploymentAgent();
    auto p = parent();
    if(p)
    {
#ifdef R64FX_DEBUG
        assert(!dynamic_cast<ModuleThreadObjectIface*>(p));
#endif//R64FX_DEBUG
        agent->parent_impl_handle = (ModuleThreadObjectImplHandle*) p;
    }
    else
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG
        auto sp = sd->newSyncPort();
#ifdef R64FX_DEBUG
        assert(sp != nullptr);
#endif//R64FX_DEBUG
        agent->sync_port = sp;
    }
    return agent;
}


void ModuleThreadObjectIface::deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent)
{
    deleteModuleDeploymentAgent(static_cast<ModuleDeploymentAgent*>(agent));
}


ThreadObjectWithdrawalAgent* ModuleThreadObjectIface::newWithdrawalAgent()
{
    auto agent = newModuleWithdrawalAgent();
    return agent;
}


void ModuleThreadObjectIface::deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent)
{
    deleteModuleWithdrawalAgent(static_cast<ModuleWithdrawalAgent*>(agent));
}


Module::Module()
{

}


Module::~Module()
{

}


ModuleConnection::ModuleConnection(Module* source_module, ModuleSource* source_port, Module* sink_module, ModuleSink* sink_port)
{
    
}

}//namespace r64fx
