#include "Module.hpp"
#include "ModulePrivate.hpp"
#include "Module_SoundDriver.hpp"
#include "SoundDriver.hpp"
#include "SignalNode_BufferRW.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "InstanceCounter.hpp"

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
    SoundDriverPortGroup*  sdpg         = nullptr;
    long                   buffer_size  = 0;
    long                   sample_rate  = 0;
};

struct RootModuleWithdrawalArgs{
    SoundDriverPortGroup* sdpg = nullptr;
};

R64FX_DECL_MODULE_AGENTS(RootModule);


struct ModuleLinkMessage{
    enum class Key : unsigned long { Link, Unlink };

    struct Item{
        SignalSource* source  = nullptr;
        SignalSink*   sink    = nullptr;
    };
    Item*                  items     = nullptr;
    unsigned long          nitems    = 0;

    ModuleLink::Callback*  callback  = nullptr;
    void*                  arg       = nullptr;
    ModuleLink**           links     = nullptr;
    unsigned int           nlinks    = 0;
};


/*
 * === Worker Thread ===================================================================
 */

struct GraphOutput : public LinkedList<GraphOutput>::Node{
    SignalNode* node = nullptr;
    GraphOutput(SignalNode* node) : node(node) {}
};

struct ModuleThreadAssets{
    SoundDriverPortGroup*    sdpg            = nullptr;
    long                     sample_rate     = 0;
    long                     flags           = 0;
    SignalGraph              signal_graph;

    LinkedList<GraphOutput>  graph_outputs;
};


class RootModuleThreadObjectImpl : public ModuleThreadObjectImpl{
    ModuleThreadAssets m;

public:
    RootModuleThreadObjectImpl(RootModuleDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        m.sdpg         = agent->sdpg;
        m.sample_rate  = agent->sample_rate;
        m.signal_graph.setFrameCount(agent->buffer_size);
    }

    void storeWithdrawalArgs(RootModuleWithdrawalAgent* agent)
        { agent->sdpg = m.sdpg; }

    ~RootModuleThreadObjectImpl(){}

    inline void readMessagesFromIface() { ThreadObjectImpl::readMessagesFromIface(); }

    inline ModuleThreadAssets* assets() { return &m; }

private:
    virtual void runThread() override final
    {
        m.sdpg->enable();

        m.flags |= (R64FX_MODULE_THREAD_RUNNING | R64FX_GRAPH_REBUILD_ARMED);
        while(m.flags & R64FX_MODULE_THREAD_RUNNING)
        {
            m.flags |= R64FX_GRAPH_REBUILD_ARMED;

            if(m.sdpg->sync())
            {
                readMessagesFromIface();

                if(m.flags & R64FX_GRAPH_REBUILD_ARMED)
                {
                    m.flags &= ~R64FX_GRAPH_REBUILD_ARMED;
                    m.signal_graph.beginBuild();
                    for(auto output : m.graph_outputs)
                    {
                        m.signal_graph.buildNode(output->node);
                    }
                    m.signal_graph.endBuild();
                }
                m.signal_graph.run();
                m.sdpg->done();
            }
            else
            {
                sleep_nanoseconds(300 * 1000);
            }
        }

        m.sdpg->disable();
    }

    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) override final
    {
        switch(msg.key())
        {
            case (unsigned long)ModuleLinkMessage::Key::Link:
            {
                auto mlm = (ModuleLinkMessage*)msg.value();
                std::cout << "Link: " << mlm->nitems << "\n";
                for(unsigned int i=0; i<mlm->nitems; i++)
                    { m.signal_graph.link(mlm->items[i].source, mlm->items[i].sink); }
                break;
            }

            case (unsigned long)ModuleLinkMessage::Key::Unlink:
            {
                auto mlm = (ModuleLinkMessage*)msg.value();
                std::cout << "Unlink: " << mlm->nitems << "\n";
                for(unsigned int i=0; i<mlm->nitems; i++)
                    { m.signal_graph.unlink(mlm->items[i].sink);}
                break;
            }

            default:
            {
                std::cerr << "Bad message type!\n";
                abort();
            }
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
{}


ModuleThreadObjectImpl::~ModuleThreadObjectImpl()
{}


long ModuleThreadObjectImpl::bufferSize() const
{
    return R64FX_MODULE_THREAD_ASSETS->signal_graph.frameCount();
}


long ModuleThreadObjectImpl::sampleRate() const
{
    return R64FX_MODULE_THREAD_ASSETS->sample_rate;
}


SoundDriverPortGroup* ModuleThreadObjectImpl::soundDriverPortGroup() const
{
    return R64FX_MODULE_THREAD_ASSETS->sdpg;
}


SignalGraph* ModuleThreadObjectImpl::signalGraph() const
{
    return &(R64FX_MODULE_THREAD_ASSETS->signal_graph);
}


void ModuleThreadObjectImpl::addGraphOutput(SignalNode* node)
{
    R64FX_MODULE_THREAD_ASSETS->graph_outputs.append(allocObj<GraphOutput>(node));
}


void ModuleThreadObjectImpl::removeGraphOutput(SignalNode* node)
{
    auto go = R64FX_MODULE_THREAD_ASSETS->graph_outputs.first();
    while(go && go->node != node) go = go->next();
    R64FX_MODULE_THREAD_ASSETS->graph_outputs.remove(go);
    freeObj(go);
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


class ModuleSoundDriverThreadObjectImpl : public ModuleThreadObjectImpl{
public:
    using ModuleThreadObjectImpl::ModuleThreadObjectImpl;
};


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
        agent->sdpg         = sd->newPortGroup();
        agent->buffer_size  = sd->bufferSize();
        agent->sample_rate  = sd->sampleRate();
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
        R64FX_DEBUG_ASSERT(dynamic_cast<RootModuleWithdrawalAgent*>(agent));
        auto withdrawal_agent = static_cast<RootModuleWithdrawalAgent*>(agent);

        auto sd = soundDriver();
        R64FX_DEBUG_ASSERT(sd);
        R64FX_DEBUG_ASSERT(withdrawal_agent->sdpg);
        sd->deletePortGroup(withdrawal_agent->sdpg);
        delete withdrawal_agent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        switch(msg.key())
        {
            case (unsigned long)ModuleLinkMessage::Key::Link:
            case (unsigned long)ModuleLinkMessage::Key::Unlink:
            {
                auto mlm = (ModuleLinkMessage*) msg.value();
                if(mlm->callback)
                    mlm->callback(mlm->links, mlm->nlinks, mlm->arg);
                delete[] mlm->items;
                delete mlm;
                break;
            }

            default:
            {
                std::cerr << "Bad message type!\n";
                abort();
            }
        }
    }

public:
    inline void sendToImpl(ModuleLinkMessage::Key key, ModuleLinkMessage* mlm)
    {
         ThreadObjectMessage msg((unsigned long)key, mlm);
         sendMessagesToImpl(&msg, 1);
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
        m_sound_driver = SoundDriver::newInstance(SoundDriver::Type::Stub, "r64fx");
        m_sound_driver->enable();

        m_timer = new(std::nothrow) Timer;
        m_timer->setInterval(5000 * 1000);
        m_timer->onTimeout([](Timer* timer, void* arg){
            auto sd = (SoundDriver*) arg;
            sd->sync();
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

    inline void changeModuleLinks(
        ModuleLinkMessage::Key key, ModuleLink** links, unsigned int nlinks, ModuleLink::Callback* callback, void* arg
    )
    {
        R64FX_DEBUG_ASSERT(links);
        R64FX_DEBUG_ASSERT(nlinks > 0);

        ModuleThreadHandle* thread = links[0]->source()->thread();
        R64FX_DEBUG_ASSERT(thread);
        R64FX_DEBUG_ASSERT(links[0]->sink()->thread() == thread);

        auto mlm       = new ModuleLinkMessage;
        mlm->items     = new ModuleLinkMessage::Item[nlinks];
        mlm->nitems    = nlinks;
        mlm->callback  = callback;
        mlm->arg       = arg;
        mlm->links     = links;
        mlm->nlinks    = nlinks;

        for(unsigned int i=0; i<nlinks; i++)
        {
            auto link = links[i];
            R64FX_DEBUG_ASSERT(link->source()->thread() == thread);
            R64FX_DEBUG_ASSERT(link->sink()->thread() == thread);

            auto source_impl = ModuleSignalSourceImpl::From(link->source());
            auto sink_impl   = ModuleSignalSinkImpl::From(link->sink());

            mlm->items[i].source = source_impl->signal_port;
            mlm->items[i].sink = sink_impl->signal_port;
        }

        ((RootModuleThreadObjectIface*)thread)->sendToImpl(key, mlm);
    }
} g_Module;

}//namespace


SoundDriver* ModuleThreadObjectIface::soundDriver()
{
    return g_Module.soundDriver();
}


ModuleThreadHandle* ModuleThreadObjectIface::thread() const
{
    R64FX_DEBUG_ASSERT(dynamic_cast<RootModuleThreadObjectIface*>(parent()));
    return (ModuleThreadHandle*)parent();
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


Module::Module()
    { g_Module.created(); }

Module::~Module()
    { g_Module.destroyed(); }

void Module::genThreads(ModuleThreadHandle** threads, int nthreads)
    { g_Module.genThreads(threads, nthreads); }

int Module::freeThreads(ModuleThreadHandle** threads, int nthreads)
    { return g_Module.freeThreads(threads, nthreads); }

ModuleThreadHandle* ModulePort::thread()
{
    return ModulePortImpl::From(this)->thread;
}

void ModuleLink::enable(ModuleLink** links, unsigned int nlinks, ModuleLink::Callback* callback, void* arg)
    { g_Module.changeModuleLinks(ModuleLinkMessage::Key::Link, links, nlinks, callback, arg); }

void ModuleLink::disable(ModuleLink** links, unsigned int nlinks, ModuleLink::Callback* callback, void* arg)
    { g_Module.changeModuleLinks(ModuleLinkMessage::Key::Unlink, links, nlinks, callback, arg); }

}//namespace r64fx

#include "Module_SoundDriver.cxx"
