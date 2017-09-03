#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "SignalGraph.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
#endif//R64FX_DEBUG

namespace r64fx{

/*
 * === Impl ===================================================================
 */

namespace{

struct ModuleImplFun : public LinkedList<ModuleImplFun>::Node{
    void (*fun)(void* arg)        = nullptr;
    void*  arg                    = nullptr;
    ModuleThreadObjectImpl* impl  = nullptr;

    ModuleImplFun(void (*fun)(void* arg), void* arg, ModuleThreadObjectImpl* impl)
    : fun(fun), arg(arg), impl(impl) {}
};


struct ModuleImplSharedAssets{
    SoundDriverSyncPort*       sync_port      = nullptr;
    LinkedList<ModuleImplFun>  prologue_list;
    LinkedList<ModuleImplFun>  epilogue_list;
    long                       buffer_size    = 0;
    long                       sample_rate    = 0;
    long                       flags          = 0;
    SignalGraph                graph;
};

}//namespace


ModuleThreadObjectImpl::ModuleThreadObjectImpl(ModuleDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
: ThreadObjectImpl(R64FX_THREAD_OBJECT_IMPL_ARGS)
, m_parent(agent->parent_impl)
{
    if(m_parent == nullptr)
    {
#ifdef R64FX_DEBUG
        assert(asset() == nullptr);
#endif//R64FX_DEBUG
        auto shared = allocObj<ModuleImplSharedAssets>();
        shared->sync_port    = agent->sync_port;
        shared->buffer_size  = agent->buffer_size;
        shared->sample_rate  = agent->sample_rate;
        setAsset(shared);
    }
}


ModuleThreadObjectImpl::~ModuleThreadObjectImpl()
{
    setPrologue(nullptr);
    setEpilogue(nullptr);
    if(m_parent == nullptr)
    {
#ifdef R64FX_DEBUG
        assert(asset() != nullptr);
#endif//R64FX_DEBUG
        freeObj((ModuleImplSharedAssets*)asset());
        setAsset(nullptr);
    }
}


void set_item(LinkedList<ModuleImplFun> &item_list, HeapAllocator* ha, void (*fun)(void* arg), void* arg, ModuleThreadObjectImpl* impl)
{
    for(auto item : item_list)
    {
        if(item->impl == impl)
        {
            if(fun)
            {
                item->fun = fun;
                item->arg = arg;
            }
            else
            {
                item_list.remove(item);
                ha->freeObj(item);
            }
            return;
        }
    }

    if(fun)
    {
        auto item = ha->allocObj<ModuleImplFun>(fun, arg, impl);
        item_list.append(item);
    }
}


SoundDriverSyncPort* ModuleThreadObjectImpl::syncPort()
{
#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    return shared->sync_port;
}


void ModuleThreadObjectImpl::setPrologue(void (*fun)(void* arg), void* arg)
{
#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    set_item(shared->prologue_list, heapAllocator(), fun, arg, this);
}


void ModuleThreadObjectImpl::setEpilogue(void (*fun)(void* arg), void* arg)
{
#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    set_item(shared->epilogue_list, heapAllocator(), fun, arg, this);
}


long ModuleThreadObjectImpl::bufferSize() const
{
#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    return shared->buffer_size;
}


long ModuleThreadObjectImpl::sampleRate() const
{
#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    return shared->sample_rate;
}


SignalGraph* ModuleThreadObjectImpl::signalGraph() const
{
#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    return &(shared->graph);
}


void ModuleThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{

}


void ModuleThreadObjectImpl::runThread()
{
    SoundDriverSyncMessage sync_msg[R64FX_SOUND_DRIVER_SYNC_PORT_BUFFER_SIZE];

#ifdef R64FX_DEBUG
    assert(asset() != nullptr);
#endif//R64FX_DEBUG
    auto shared = (ModuleImplSharedAssets*) asset();
    shared->sync_port->enable();

    m_flags |= R64FX_MODULE_IMPL_THREAD_RUNNING;
    while(m_flags & R64FX_MODULE_IMPL_THREAD_RUNNING)
    {
        readMessagesFromIface();
        long nmsgs = shared->sync_port->readMessages(sync_msg, R64FX_SOUND_DRIVER_SYNC_PORT_BUFFER_SIZE);
        if(nmsgs > 0)
        {
            for(auto item : shared->prologue_list)
            {
                item->fun(item->arg);
            }

            shared->graph.run(shared->buffer_size);

            for(auto item : shared->epilogue_list)
            {
                item->fun(item->arg);
            }
        }
        else
        {
            sleep_nanoseconds(3000 * 1000);
        }
    }

    shared->sync_port->disable();
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
    auto impl = deployModuleImpl(ha, R64FX_THREAD_OBJECT_IMPL_ARGS);
    return impl;
}


void ModuleWithdrawalAgent::withdrawImpl(HeapAllocator* ha, ThreadObjectImpl* impl)
{
    auto module_impl = static_cast<ModuleThreadObjectImpl*>(impl);
    sync_port = module_impl->syncPort();
    withdrawModuleImpl(ha, module_impl);
}

/*
 * === Iface ==================================================================
 */
namespace{

SoundDriver*  g_SoundDriver            = nullptr;
Timer*        g_SoundDriverEventTimer  = nullptr;
long          g_SoundDriverUserCount   = 0;

}//namespace


SoundDriver* ModuleThreadObjectIface::soundDriver()
{
    if(!g_SoundDriver)
    {
        g_SoundDriver = SoundDriver::newInstance(SoundDriver::Type::Stub);
        g_SoundDriver->enable();
    }
    return g_SoundDriver;
}


void freeSoundDriver()
{
    g_SoundDriver->disable();
    SoundDriver::deleteInstance(g_SoundDriver);
    g_SoundDriver = nullptr;
}


void initSoundDriverEventTimer()
{
    if(g_SoundDriverEventTimer != nullptr)
        return;

    g_SoundDriverEventTimer = new(std::nothrow) Timer;
    g_SoundDriverEventTimer->setInterval(5000 * 1000);
    g_SoundDriverEventTimer->onTimeout([](Timer* timer, void* arg){
        g_SoundDriver->processEvents();
        if(g_SoundDriverUserCount == 0)
        {
            freeSoundDriver();
            g_SoundDriverEventTimer->stop();
            g_SoundDriverEventTimer->suicide();
            g_SoundDriverEventTimer = nullptr;
        }
    });
}


ThreadObjectDeploymentAgent* ModuleThreadObjectIface::newDeploymentAgent()
{
    auto agent = newModuleDeploymentAgent();
    if(parent())
    {
#ifdef R64FX_DEBUG
        assert(dynamic_cast<ModuleThreadObjectIface*>(parent()));
#endif//R64FX_DEBUG
        auto p = static_cast<ModuleThreadObjectIface*>(parent());
        agent->parent_impl = (ModuleThreadObjectImpl*) p->impl();
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
        agent->buffer_size = sd->bufferSize();
        agent->sample_rate = sd->sampleRate();
        g_SoundDriverUserCount++;
        initSoundDriverEventTimer();
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
    auto module_agent = static_cast<ModuleWithdrawalAgent*>(agent);
    if(!parent())
    {
        g_SoundDriver->deleteSyncPort(module_agent->sync_port);
        g_SoundDriverUserCount--;
    }

    deleteModuleWithdrawalAgent(module_agent);
}



#define R64FX_MODULE_PORT_IS_SOURCE 1


ModulePort::ModulePort()
{

}


bool ModulePort::isSource() const
{
    return m_flags & R64FX_MODULE_PORT_IS_SOURCE;
}


ModuleSink::ModuleSink()
{

}


ModuleSource::ModuleSource()
{
    m_flags |= R64FX_MODULE_PORT_IS_SOURCE;
}


Module::Module()
{

}


Module::~Module()
{

}


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
