#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "InstanceCounter.hpp"
#include "SignalGraph.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
#endif//R64FX_DEBUG

#ifdef R64FX_DEBUG
#define R64FX_MODULE_THREAD_ASSETS\
    (assert(rootImpl()), assert(dynamic_cast<RootModuleThreadObjectImpl*>(rootImpl())),\
    static_cast<RootModuleThreadObjectImpl*>(rootImpl())->assets())
#else
#define R64FX_MODULE_THREAD_ASSETS (static_cast<RootModuleThreadObjectImpl*>(rootImpl())->assets())
#endif

#define R64FX_MODULE_THREAD_RUNNING 1
#define R64FX_GRAPH_REBUILD_ARMED   2

#define R64FX_MODULE_PORT_IS_SOURCE 1

using namespace std;

namespace r64fx{

/*
 * === Agents & Messages ==============================================================
 */

namespace{

struct RootModuleDeploymentArgs{
    SoundDriverSyncPort* sd_sync_port = nullptr;
    long buffer_size = 0;
    long sample_rate = 0;
};

struct RootModuleWithdrawalArgs{
    SoundDriverSyncPort* sd_sync_port = nullptr;
};

R64FX_DECL_MODULE_AGENTS(RootModule);

struct ModuleConnectionMessage{
    SignalSource* source         = nullptr;
    SignalSink*   sink           = nullptr;
};


/*
 * === Worker Thread ===================================================================
 */

struct ModuleThreadAssets{
    SoundDriverSyncPort*  sd_sync_port   = nullptr;
    long                  sample_rate    = 0;
    long                  flags          = 0;
    SignalGraph           signal_graph;

    struct Fun : public LinkedList<Fun>::Node{
        void (*fun)(void* arg)        = nullptr;
        void*  arg                    = nullptr;
        ModuleThreadObjectImpl* impl  = nullptr;

        Fun(void (*fun)(void* arg), void* arg, ModuleThreadObjectImpl* impl)
        : fun(fun), arg(arg), impl(impl) {}
    };
    LinkedList<Fun> prologue_list;
    LinkedList<Fun> epilogue_list;
};


class RootModuleThreadObjectImpl : public ModuleThreadObjectImpl{
    ModuleThreadAssets m;

public:
    RootModuleThreadObjectImpl(RootModuleDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        m.sd_sync_port  = agent->sd_sync_port;
        m.sample_rate   = agent->sample_rate;

        m.signal_graph.setFrameCount(agent->buffer_size);
    }

    void storeWithdrawalArgs(RootModuleWithdrawalAgent* agent)
    {
        agent->sd_sync_port = m.sd_sync_port;
    }

    ~RootModuleThreadObjectImpl(){}

    inline void readMessagesFromIface()
    {
        ThreadObjectImpl::readMessagesFromIface();
    }

    inline ModuleThreadAssets* assets()
    {
        return &m;
    }

private:
    virtual void runThread() override final
    {
        m.sd_sync_port->enable();

        SignalGraphProcessor sgp;

        m.flags |= (R64FX_MODULE_THREAD_RUNNING | R64FX_GRAPH_REBUILD_ARMED);
        while(m.flags & R64FX_MODULE_THREAD_RUNNING)
        {
            readMessagesFromIface();

            SoundDriverSyncMessage sync_msg[R64FX_SOUND_DRIVER_SYNC_PORT_BUFFER_SIZE];
            long nmsgs = m.sd_sync_port->readMessages(sync_msg, R64FX_SOUND_DRIVER_SYNC_PORT_BUFFER_SIZE);
            if(nmsgs > 0)
            {
                for(auto item : m.prologue_list)
                {
                    item->fun(item->arg);
                }

                if(m.flags & R64FX_GRAPH_REBUILD_ARMED)
                {
                    sgp.build(m.signal_graph);
                    m.flags &= ~R64FX_GRAPH_REBUILD_ARMED;
                }
                sgp.run();

                for(auto item : m.epilogue_list)
                {
                    item->fun(item->arg);
                }
            }
            else
            {
                sleep_nanoseconds(3000 * 1000);
            }
        }

        m.sd_sync_port->disable();

#ifdef R64FX_DEBUG
        assert(m.prologue_list.empty());
        assert(m.epilogue_list.empty());
#endif///R64FX_DEBUG
    }

    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) override final
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
};

R64FX_DEF_MODULE_AGENTS(RootModule)

}//namespace



ModuleThreadObjectImpl::ModuleThreadObjectImpl(ModuleDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
: ThreadObjectImpl(R64FX_THREAD_OBJECT_IMPL_ARGS)
{}


void ModuleThreadObjectImpl::clearHooks()
{
    setPrologue(nullptr);
    setEpilogue(nullptr);
}


ModuleThreadObjectImpl::~ModuleThreadObjectImpl()
{
}


SoundDriverSyncPort* ModuleThreadObjectImpl::syncPort() const
{
    return R64FX_MODULE_THREAD_ASSETS->sd_sync_port;
}


namespace{

void set_list_item(LinkedList<ModuleThreadAssets::Fun> &item_list, HeapAllocator* ha, void (*fun)(void* arg), void* arg, ModuleThreadObjectImpl* impl)
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
        auto item = ha->allocObj<ModuleThreadAssets::Fun>(fun, arg, impl);
        item_list.append(item);
    }
}

}//namespace


void ModuleThreadObjectImpl::setPrologue(void (*fun)(void* arg), void* arg)
{
    set_list_item(R64FX_MODULE_THREAD_ASSETS->prologue_list, heapAllocator(), fun, arg, this);
}


void ModuleThreadObjectImpl::setEpilogue(void (*fun)(void* arg), void* arg)
{
    set_list_item(R64FX_MODULE_THREAD_ASSETS->epilogue_list, heapAllocator(), fun, arg, this);
}


long ModuleThreadObjectImpl::bufferSize() const
{
    return R64FX_MODULE_THREAD_ASSETS->signal_graph.frameCount();
}


long ModuleThreadObjectImpl::sampleRate() const
{
    return R64FX_MODULE_THREAD_ASSETS->sample_rate;
}


SignalGraph* ModuleThreadObjectImpl::signalGraph() const
{
    return &(R64FX_MODULE_THREAD_ASSETS->signal_graph);
}


void ModuleThreadObjectImpl::armRebuild()
{
    R64FX_MODULE_THREAD_ASSETS->flags |= R64FX_GRAPH_REBUILD_ARMED;
}


void ModuleThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg) {}


void ModuleThreadObjectImpl::runThread()
{
#ifdef R64FX_DEBUG
    std::cerr << "Must not run!\n";
    abort();
#endif//R64FX_DEBUG
}


void ModuleThreadObjectImpl::exitThread()
{
    R64FX_MODULE_THREAD_ASSETS->flags &= ~R64FX_MODULE_THREAD_RUNNING;
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
    withdrawModuleImpl(ha, module_impl);
}


/*
 * === Main Thread ==================================================================
 */

namespace{

class RootModuleThreadObjectIface : public ModuleThreadObjectIface{
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd);
#endif//R64FX_DEBUG

        auto agent = new RootModuleDeploymentAgent;
        agent->sd_sync_port  = sd->newSyncPort();
        agent->buffer_size   = sd->bufferSize();
        agent->sample_rate   = sd->sampleRate();
        return agent;
    }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
    {
        return new RootModuleWithdrawalAgent;
    }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
    {
#ifdef R64FX_DEBUG
        assert(dynamic_cast<RootModuleWithdrawalAgent*>(agent));
#endif//R64FX_DEBUG
        auto withdrawal_agent = static_cast<RootModuleWithdrawalAgent*>(agent);

        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd);
        assert(withdrawal_agent->sd_sync_port);
#endif//R64FX_DEBUG
        sd->deleteSyncPort(withdrawal_agent->sd_sync_port);
        delete withdrawal_agent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        auto msgs = (ModuleConnectionMessage*) msg.value();
        long nmsgs = msg.key();
#ifdef R64FX_DEBUG
        assert(msgs);
        assert(nmsgs > 0);
#endif//R64FX_DEBUG
        delete[] msgs;
    }
};


class ModuleGlobal : public InstanceCounter{
    SoundDriver* m_sound_driver  = nullptr;
    Timer*       m_timer         = nullptr;

    struct RootModuleRec : public LinkedList<RootModuleRec>::Node{
        RootModuleThreadObjectIface* iface = nullptr;
    };
    LinkedList<RootModuleRec> m_recs;

    struct EngagementArgs{
        ModuleGlobal*       global    = nullptr;
        Module*             module    = nullptr;
        ThreadObjectIface*  iface     = nullptr;
        Module::Callback*   done      = nullptr;
        void*               done_arg  = nullptr;

        EngagementArgs(ModuleGlobal* global, Module* module, ThreadObjectIface* iface, Module::Callback* done, void* done_arg)
        : global(global), module(module), iface(iface), done(done), done_arg(done_arg) {}

        inline void callBack()
        {
            if(done)
            {
                done(module, done_arg);
            }
        }
    };

    virtual void initEvent() override final
    {
        m_sound_driver = SoundDriver::newInstance(SoundDriver::Type::Stub);
        m_sound_driver->enable();

        m_timer = new(std::nothrow) Timer;
        m_timer->setInterval(5000 * 1000);
        m_timer->onTimeout([](Timer* timer, void* arg){
            auto sd = (SoundDriver*) arg;
            sd->processEvents();
        }, m_sound_driver);
    }

    virtual void cleanupEvent() override final
    {
        m_sound_driver->disable();
        delete m_sound_driver;

        m_timer->stop();
        delete m_timer;
    }

public:
    inline void deploy(ModuleThreadObjectIface* iface, ModuleThreadHandle* thread_handle, Module::Callback* done, void* done_arg, Module* module)
    {
#ifdef R64FX_DEBUG
        assert(iface);
        assert(done);
#endif//R64FX_DEBUG

        if(!thread_handle)
        {
            if(m_recs.empty())
                genThreads(&thread_handle, 1);
            else
                thread_handle = (ModuleThreadHandle*) m_recs.first();
        }

        auto rec = (RootModuleRec*) thread_handle;
        auto args = new EngagementArgs(this, module, iface, done, done_arg);
        if(rec->iface)
        {
            obtainedDeployedRootModule(rec->iface, args);
        }
        else
        {
            rec->iface = new RootModuleThreadObjectIface;
            rec->iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
                auto args = (EngagementArgs*) arg;
                args->global->obtainedDeployedRootModule((RootModuleThreadObjectIface*) iface, args);
            }, args);
        }
    }

private:
    inline void obtainedDeployedRootModule(RootModuleThreadObjectIface* root, EngagementArgs* args)
    {
        args->iface->deploy(root, [](ThreadObjectIface* iface, void* arg){
            auto args = (EngagementArgs*) arg;
            args->callBack();
            delete args;
        }, args);
    }

public:
    inline void withdraw(ModuleThreadObjectIface* iface, Module::Callback done, void* done_arg, Module* module)
    {
#ifdef R64FX_DEBUG
        assert(iface);
        assert(done);
#endif//R64FX_DEBUG

        auto args = new EngagementArgs(this, module, iface->parent(), done, done_arg);
        iface->withdraw([](ThreadObjectIface* iface, void* arg){
            auto args = (EngagementArgs*) arg;
            args->global->objectWithdrawn(iface, args);
        }, args);
    }

private:
    inline void objectWithdrawn(ThreadObjectIface* iface, EngagementArgs* args)
    {
        auto parent = args->iface;
        if((!parent->parent()) && parent->empty()) //Root is empty. Withdraw it.
        {
            parent->withdraw([](ThreadObjectIface* iface, void* arg){
                auto args = (EngagementArgs*) arg;
                args->global->rootWithdrawn(iface, args);
            }, args);
        }
        else
        {
            args->callBack();
            delete args;
        }
    }

    void rootWithdrawn(ThreadObjectIface* iface, EngagementArgs* args)
    {
#ifdef R64FX_DEBUG
        assert(dynamic_cast<RootModuleThreadObjectIface*>(iface));
#endif//R64FX_DEBUG
        auto root_iface = static_cast<RootModuleThreadObjectIface*>(iface);
        for(auto rec : m_recs)
        {
            if(rec->iface == root_iface)
            {
                rec->iface = nullptr;
                break;
            }
        }
        delete root_iface;

        args->callBack();
        delete args;
    }

public:
    inline void genThreads(ModuleThreadHandle** threads, int nthreads)
    {
        for(int i=0; i<nthreads; i++)
        {
            auto rec = new RootModuleRec;
            m_recs.append(rec);
            threads[i] = (ModuleThreadHandle*) rec;
        }
    }

    inline int freeThreads(ModuleThreadHandle** threads, int nthreads)
    {
        int nfreed = 0;
        for(int i=0; i<nthreads; i++)
        {
            auto rec = (RootModuleRec*) threads[i];
            if(rec->iface)
                continue;
            m_recs.remove(rec);
            delete rec;
            threads[i] = nullptr;
            nfreed++;
        }
        return nfreed;
    }

    inline SoundDriver* soundDriver() const { return m_sound_driver; }
} g_Module;

}//namespace


SoundDriver* ModuleThreadObjectIface::soundDriver()
{
    return g_Module.soundDriver();
}

ThreadObjectDeploymentAgent* ModuleThreadObjectIface::newDeploymentAgent()
{
    return newModuleDeploymentAgent();
}


void ModuleThreadObjectIface::deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent)
{
    deleteModuleDeploymentAgent(static_cast<ModuleDeploymentAgent*>(agent));
}


ThreadObjectWithdrawalAgent* ModuleThreadObjectIface::newWithdrawalAgent()
{
    return newModuleWithdrawalAgent();
}


void ModuleThreadObjectIface::deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent)
{
    auto module_agent = static_cast<ModuleWithdrawalAgent*>(agent);
    deleteModuleWithdrawalAgent(module_agent);
}


void ModulePrivate::deploy(ModuleThreadObjectIface* iface, ModuleThreadHandle* thread, Module::Callback done, void* done_arg, Module* module)
{
    g_Module.deploy(iface, thread, done, done_arg, module);
}


void ModulePrivate::withdraw(ModuleThreadObjectIface* iface, Module::Callback done, void* done_arg, Module* module)
{
    g_Module.withdraw(iface, done, done_arg, module);
}


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
    g_Module.created();
}


Module::~Module()
{
    g_Module.destroyed();
}


void Module::genThreads(ModuleThreadHandle** threads, int nthreads)
{
    g_Module.genThreads(threads, nthreads);
}


int Module::freeThreads(ModuleThreadHandle** threads, int nthreads)
{
    return g_Module.freeThreads(threads, nthreads);
}

}//namespace r64fx
