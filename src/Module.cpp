#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{
    SoundDriver* g_SoundDriver = nullptr;
}//namespace


/*======= Worker Thread =======*/

ModuleThreadObjectImpl::ModuleThreadObjectImpl(ThreadObjectIface* iface, ThreadObjectManagerImpl* manager_impl)
: ThreadObjectImpl(iface, manager_impl)
{
    if(payload() == nullptr)
    {
        setPayload(new ModuleThreadAssets);
    }

    auto assets = threadAssets();
    assets->count++;
}


ModuleThreadObjectImpl::~ModuleThreadObjectImpl()
{
    auto assets = threadAssets();
#ifdef R64FX_DEBUG
    assert(assets != nullptr);
#endif//R64FX_DEBUG
    assets->count--;
    if(assets->count == 0)
    {
        delete assets;
        setPayload(nullptr);
    }
}


ModuleThreadAssets* ModuleThreadObjectImpl::threadAssets() const
{
    return (ModuleThreadAssets*) payload();
}


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
        sleep_nanoseconds(1000 * 1000);
    }
}


void ModuleThreadObjectImpl::exitThread()
{
    m_flags &= ~R64FX_MODULE_IMPL_THREAD_RUNNING;
}


/*======= Agents =======*/

ModuleThreadObjectDeploymentAgent::ModuleThreadObjectDeploymentAgent()
{
    auto sd = ModuleGlobal::soundDriver();
#ifdef R64FX_DEBUG
    assert(sd != nullptr);
#endif//R64FX_DEBUG
    m_sync_port = sd->newSyncPort();
#ifdef R64FX_DEBUG
    assert(m_sync_port != nullptr);
#endif//R64FX_DEBUG
}



/*======= Main Thread =======*/

void ModuleThreadObjectIface::deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent)
{
    delete agent;
}


void ModuleThreadObjectIface::deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent)
{
    delete agent;
}


SoundDriver* ModuleGlobal::soundDriver()
{
    if(!g_SoundDriver)
    {
        g_SoundDriver = SoundDriver::newInstance(SoundDriver::Type::Jack);
    }
    return g_SoundDriver;
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
