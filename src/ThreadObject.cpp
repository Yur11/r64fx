#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "ThreadObjectFlags.hpp"
#include "CircularBuffer.hpp"
#include "Thread.hpp"
#include "Timer.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
using std::cout;
using std::cerr;
#endif//R64FX_DEBUG

namespace r64fx{

namespace{

/* Comm. protocol between iface. & impl. */
enum{
    DeployObject,
    ObjectDeployed,
    WithdrawObject,
    ObjectWithdrawn,
    TerminateThread,
    ThreadTerminating,

    PickDestination  = 0xFFFFFFFFFFFFFFFF
};

}


struct ThreadObjectManagerIface{
    Thread* m_thread = nullptr;

    Timer* m_timer = nullptr;

    CircularBuffer<ThreadObjectMessage>* m_to_impl   = nullptr;
    CircularBuffer<ThreadObjectMessage>* m_from_impl = nullptr;

    ThreadObjectImpl*  m_dst_impl  = nullptr;
    ThreadObjectIface* m_dst_iface = nullptr;

    void deployThread(ThreadObjectIface* root_iface);

    void sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromImpl();

    void deployObject(
        ThreadObjectIface* object,
        ThreadObjectCallbackFun done, void* done_arg,
        ThreadObjectDeploymentAgent* parent_agent
    );

    void objectDeployed(ThreadObjectDeploymentAgent* agent);

    void withdrawObject(
        ThreadObjectIface* object,
        ThreadObjectCallbackFun done, void* done_arg,
        ThreadObjectWithdrawalAgent* parent_agent
    );

    void objectWithdrawn(ThreadObjectWithdrawalAgent* agent);

    void dispatchMessageFromImpl(const ThreadObjectMessage &msg);

    void suicide();

    /* Set or clear two groups of flags of a ThreadObjectIface subtree. */
    static void alterTreeFlags(ThreadObjectIface* iface, unsigned long f1, bool s1)
    {
        if(s1)
            iface->m_flags |= f1;
        else
            iface->m_flags &= ~f1;

        for(auto child : iface->children())
            alterTreeFlags(child, f1, s1);
    }

    /* Set or clear two groups of flags of a ThreadObjectIface subtree. */
    static void alterTreeFlags(ThreadObjectIface* iface, unsigned long f1, bool s1, unsigned long f2, bool s2)
    {
        if(s1)
            iface->m_flags |= f1;
        else
            iface->m_flags &= ~f1;

        if(s2)
            iface->m_flags |= f2;
        else
            iface->m_flags &= ~f2;

        for(auto child : iface->children())
            alterTreeFlags(child, f1, s1, f2, s2);
    }
};


struct ThreadObjectManagerImpl{
    CircularBuffer<ThreadObjectMessage>* m_to_iface   = nullptr;
    CircularBuffer<ThreadObjectMessage>* m_from_iface = nullptr;
    ThreadObjectIface* m_dst_iface = nullptr;
    ThreadObjectImpl*  m_dst_impl  = nullptr;

    ThreadObjectExecAgent* m_exec_agent = nullptr;

    ThreadObjectManagerImpl(CircularBuffer<ThreadObjectMessage>* to_iface, CircularBuffer<ThreadObjectMessage>* from_iface)
    : m_to_iface(to_iface)
    , m_from_iface(from_iface)
    {
        
    }

    void sendMessagesToIface(ThreadObjectIface* dst_iface, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromIface();

    void dispatchMessageFromIface(const ThreadObjectMessage &msg);
};


ThreadObjectIface::ThreadObjectIface()
{
    
}


ThreadObjectIface::~ThreadObjectIface()
{
    
}


void ThreadObjectIface::deploy(ThreadObjectIface* parent, ThreadObjectCallbackFun done_fun, void* done_arg)
{
#ifdef R64FX_DEBUG
    assert(!isPending());
    assert(!isDeployed());
    assert(m_parent == nullptr);
#endif//R64FX_DEBUG

    if(parent)
    {
#ifdef R64FX_DEBUG
        assert(!parent->isPending());
#endif//R64FX_DEBUG
        if(parent->isDeployed())
        {
            m_manager = parent->m_manager;
        }
        parent->m_children.append(this);
        m_parent = parent;
    }
    else
    {
        m_manager = new ThreadObjectManagerIface;
        m_manager->deployThread(this);

        m_flags |= R64FX_THREAD_OBJECT_IS_ROOT;
    }

    if(m_manager)
    {
        ThreadObjectManagerIface::alterTreeFlags(this, R64FX_THREAD_OBJECT_PENDING, true);
        m_manager->deployObject(this, done_fun, done_arg, nullptr);
    }
}


void ThreadObjectManagerIface::deployObject(
    ThreadObjectIface* object,
    ThreadObjectCallbackFun done_fun, void* done_arg,
    ThreadObjectDeploymentAgent* parent_agent
)
{
    object->m_manager = this;

    auto agent = object->newDeploymentAgent();
#ifdef R64FX_DEBUG
    assert(agent != nullptr);
#endif//R64FX_DEBUG
    agent->object_iface  = object;
    agent->done_fun      = done_fun;
    agent->done_arg      = done_arg;
    agent->parent_agent  = parent_agent;

    ThreadObjectMessage msg(DeployObject, agent);
    sendMessagesToImpl(nullptr, &msg, 1);
}


void ThreadObjectManagerIface::objectDeployed(ThreadObjectDeploymentAgent* agent)
{
    bool done = false;
    ThreadObjectCallbackFun done_fun = nullptr;
    void* done_arg = nullptr;

    auto object =  agent->object_iface;
    object->m_deployed_impl = agent->deployed_impl;
    if(object->m_children.isEmpty())
    {
        auto parent_agent = agent->parent_agent;
        for(;;)
        {
            object->deleteDeploymentAgent(agent);

            if(parent_agent)
            {
                auto sibling = object->next();
                if(sibling)
                {
                    deployObject(sibling, nullptr, nullptr, parent_agent);
                    break;
                }
                else
                {
                    object = parent_agent->object_iface;
                    if(!parent_agent->parent_agent)
                    {
                        done_fun = parent_agent->done_fun;
                        done_arg = parent_agent->done_arg;  
                    }
                    agent = parent_agent;
                    parent_agent = parent_agent->parent_agent;
                    continue;
                }
            }
            else
            {
                done = true;
                break;
            }
        }
    }
    else
    {
        deployObject(
            object->m_children.first(), nullptr, nullptr, agent
        );
    }

    if(done)
    {
        alterTreeFlags(
            object, 
            R64FX_THREAD_OBJECT_DEPLOYED, true, 
            R64FX_THREAD_OBJECT_PENDING, false
        );

        if(done_fun)
        {
            done_fun(object, done_arg);
        }
    }
}


void ThreadObjectIface::withdraw(ThreadObjectCallbackFun done_fun, void* done_arg)
{
    if(isDeployed())
    {
        ThreadObjectManagerIface::alterTreeFlags(this, R64FX_THREAD_OBJECT_PENDING, true);
        m_manager->withdrawObject(this, done_fun, done_arg, nullptr);
    }
    else
    {
        m_parent->m_children.remove(this);
        m_parent = nullptr;
    }
}


void ThreadObjectManagerIface::withdrawObject(
    ThreadObjectIface* object,
    ThreadObjectCallbackFun done_fun, void* done_arg,
    ThreadObjectWithdrawalAgent* parent_agent
)
{
    for(;;)
    {
        auto agent = object->newWithdrawalAgent();
#ifdef R64FX_DEBUG
        assert(agent != nullptr);
#endif//R64FX_DEBUG
        agent->object_iface    =  object;
        agent->withdrawn_impl  =  object->m_deployed_impl;
        agent->done_fun        =  done_fun;
        agent->done_arg        =  done_arg;
        agent->parent_agent    =  parent_agent;
        parent_agent           =  agent;

        if(object->m_children.isEmpty())
        {
            ThreadObjectMessage msg(WithdrawObject, agent);
            sendMessagesToImpl(nullptr, &msg, 1);
            break;
        }
        else
        {
            object = object->m_children.last();
        }
    }
}


void ThreadObjectManagerIface::objectWithdrawn(ThreadObjectWithdrawalAgent* agent)
{
    auto object = agent->object_iface;
    auto parent_agent = agent->parent_agent;
    if(parent_agent)
    {
        object->deleteWithdrawalAgent(agent);

        auto sibling = object->prev();
        if(sibling)
        {
            withdrawObject(sibling, nullptr, nullptr, parent_agent);
        }
        else
        {
            ThreadObjectMessage msg(WithdrawObject, parent_agent);
            sendMessagesToImpl(nullptr, &msg, 1);
        }
    }
    else
    {
        alterTreeFlags(object, (R64FX_THREAD_OBJECT_PENDING | R64FX_THREAD_OBJECT_DEPLOYED), false);
        if(!object->m_parent)
        {
            ThreadObjectMessage msg(TerminateThread);
            sendMessagesToImpl(nullptr, &msg, 1);
        }

        auto done_fun = agent->done_fun;
        auto done_arg = agent->done_arg;
        object->deleteWithdrawalAgent(agent);
        if(done_fun)
        {
            done_fun(object, done_arg);
        }
    }
    object->m_manager = nullptr;
}


inline ThreadObjectIface* ThreadObjectIface::parent() const
{
    return m_parent;
}


inline IteratorPair<ThreadObjectIfaceIterator> ThreadObjectIface::children() const
{
    return {m_children.begin(), m_children.end()};
}


inline bool ThreadObjectIface::isDeployed() const
{
    return m_flags & R64FX_THREAD_OBJECT_DEPLOYED;
}


inline bool ThreadObjectIface::isPending() const
{
    return m_flags & R64FX_THREAD_OBJECT_PENDING;
}


inline bool ThreadObjectIface::deploymentPending() const
{
    return (!isDeployed()) && isPending();
}


inline bool ThreadObjectIface::withdrawalPending() const
{
    return isDeployed() && isPending();
}


bool ThreadObjectIface::isThreadRoot() const
{
    return m_flags & R64FX_THREAD_OBJECT_IS_ROOT;
}


void ThreadObjectIface::sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(m_manager != nullptr);
    assert(m_deployed_impl != nullptr);
#endif//R64FX_DEBUG
    m_manager->sendMessagesToImpl(m_deployed_impl, msgs, nmsgs);
}


inline void ThreadObjectManagerIface::deployThread(ThreadObjectIface* root_iface)
{
    auto agent = root_iface->newExecAgent();
    agent->root_iface = root_iface;
    
    struct Args{
        ThreadObjectExecAgent*                agent;
        CircularBuffer<ThreadObjectMessage>*  iface_to_impl;
        CircularBuffer<ThreadObjectMessage>*  impl_to_iface;
    };
    auto args = new Args;
    args->agent          =  agent;
    args->iface_to_impl  =  m_to_impl    = new CircularBuffer<ThreadObjectMessage>(32);
    args->impl_to_iface  =  m_from_impl  = new CircularBuffer<ThreadObjectMessage>(32);

    m_thread = new Thread;
    m_thread->run([](void* arg) -> void* {
        auto args = (Args*) arg;
        auto agent = args->agent;
        auto manager = new ThreadObjectManagerImpl(args->impl_to_iface, args->iface_to_impl);
        delete args;
        agent->m_manager_impl = manager;
        manager->m_exec_agent = agent;
        agent->exec();
        ThreadObjectMessage msg(ThreadTerminating, agent);
        manager->sendMessagesToIface(nullptr, &msg, 1);
        delete manager;
        return nullptr;
    }, args);

    m_timer = new Timer;
    m_timer->onTimeout([](Timer* timer, void* arg){
        auto self = (ThreadObjectManagerIface*) arg;
        self->readMessagesFromImpl();
    }, this);
    m_timer->setInterval(1000);
    m_timer->start();
}


inline void ThreadObjectManagerIface::sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs)
{
    if(m_dst_impl != dst_impl)
    {
        ThreadObjectMessage msg(PickDestination, dst_impl);
        m_dst_impl = dst_impl;
        m_to_impl->write(&msg, 1);
    }
    m_to_impl->write(msgs, nmsgs);
}


inline void ThreadObjectManagerIface::readMessagesFromImpl()
{
    ThreadObjectMessage msg;
    while(m_from_impl->read(&msg, 1))
    {
        if(msg.key() == PickDestination)
        {
            m_dst_iface = (ThreadObjectIface*) msg.value();
        }
        else
        {
            if(m_dst_iface == nullptr)
            {
                if(msg.key() == ObjectDeployed)
                {
                    objectDeployed((ThreadObjectDeploymentAgent*) msg.value());
                }
                else if(msg.key() == ObjectWithdrawn)
                {
                    objectWithdrawn((ThreadObjectWithdrawalAgent*) msg.value());
                }
                else if(msg.key() == ThreadTerminating)
                {
                    auto exec_agent = (ThreadObjectExecAgent*) msg.value();
                    auto root_iface = exec_agent->root_iface;
                    root_iface->deleteExecAgent(exec_agent);
                    suicide();
                    return;
                }
            }
            else
            {
                m_dst_iface->messageFromImplRecieved(msg);
            }
        }
    }
}


void ThreadObjectManagerIface::suicide()
{
    m_timer->stop();
    m_timer->suicide();
    delete m_timer;
    m_thread->join();
    delete m_thread;
    delete m_to_impl;
    delete m_from_impl;
    delete this;
}


ThreadObjectImpl::ThreadObjectImpl(ThreadObjectIface* iface)
: m_iface(iface)
{

}


ThreadObjectImpl::~ThreadObjectImpl()
{
    
}


inline void ThreadObjectImpl::sendMessagesToIface(ThreadObjectMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(m_iface != nullptr);
#endif//R64FX_DEBUG
    m_manager->sendMessagesToIface(m_iface, msgs, nmsgs);
}


inline void ThreadObjectManagerImpl::sendMessagesToIface(ThreadObjectIface* dst_iface, ThreadObjectMessage* msgs, int nmsgs)
{
    if(m_dst_iface != dst_iface)
    {
        ThreadObjectMessage msg(PickDestination, dst_iface);
        m_to_iface->write(&msg, 1);
        m_dst_iface = dst_iface;
    }
    m_to_iface->write(msgs, nmsgs);
}


void ThreadObjectExecAgent::readMessagesFromIface()
{
    m_manager_impl->readMessagesFromIface();
}


inline void ThreadObjectManagerImpl::readMessagesFromIface()
{
    ThreadObjectMessage msg;
    while(m_from_iface->read(&msg, 1))
    {
        if(msg.key() == PickDestination)
        {
            m_dst_impl = (ThreadObjectImpl*) msg.value();
        }
        else if(m_dst_impl == nullptr)
        {
            dispatchMessageFromIface(msg);
        }
        else
        {
            m_dst_impl->messageFromIfaceRecieved(msg);
        }
    }
}


inline void ThreadObjectManagerImpl::dispatchMessageFromIface(const ThreadObjectMessage &msg)
{
    if(msg.key() == DeployObject)
    {
        auto agent = (ThreadObjectDeploymentAgent*) msg.value();
        agent->deployed_impl = agent->deployImpl(agent->object_iface);
        ThreadObjectMessage response_msg(ObjectDeployed, agent);
        sendMessagesToIface(nullptr, &response_msg, 1);
    }
    else if(msg.key() == WithdrawObject)
    {
        auto agent = (ThreadObjectWithdrawalAgent*) msg.value();
        agent->withdrawImpl(agent->withdrawn_impl);
        ThreadObjectMessage response_msg(ObjectWithdrawn, agent);
        sendMessagesToIface(nullptr, &response_msg, 1);
    }
    else if(msg.key() == TerminateThread)
    {
        m_exec_agent->terminate();
    }
    else
    {
#ifdef R64FX_DEBUG
        cerr << "ThreadObjectManagerImpl: Bad Message!\n";
        abort();
#endif//R64FX_DEBUG
    }
}

}//namespace r64fx
