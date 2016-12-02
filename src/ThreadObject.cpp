#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "ThreadObjectFlags.hpp"
#include "ThreadObjectMessage.hpp"
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

    ThreadObjectManagerIface(CircularBuffer<ThreadObjectMessage>* to_impl, CircularBuffer<ThreadObjectMessage>* from_impl)
    : m_to_impl(to_impl)
    , m_from_impl(from_impl)
    {
        m_thread = new Thread;
        m_timer = new Timer;
    }

    ~ThreadObjectManagerIface()
    {
        delete m_thread;
        delete m_timer;
    }

    void runThread(ThreadObjectExecAgent* agent);

    void sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromImpl();

    void deployObject(
        ThreadObjectIface* parent, ThreadObjectIface* child,
        ThreadObjectCallbackFun done, void* done_arg,
        ThreadObjectDeploymentAgent* parent_agent = nullptr
    );

    void objectDeployed(ThreadObjectDeploymentAgent* agent);

    void withdrawObject(
        ThreadObjectIface* parent, ThreadObjectIface* child,
        ThreadObjectCallbackFun done, void* done_arg,
        ThreadObjectWithdrawalAgent* parent_agent = nullptr
    );

    void objectWithdrawn(ThreadObjectWithdrawalAgent* agent);

    void dispatchMessageFromImpl(const ThreadObjectMessage &msg);
};


struct ThreadObjectManagerImpl{
    CircularBuffer<ThreadObjectMessage>* m_to_iface   = nullptr;
    CircularBuffer<ThreadObjectMessage>* m_from_iface = nullptr;
    ThreadObjectIface* m_dst_iface = nullptr;
    ThreadObjectImpl*  m_dst_impl  = nullptr;

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


void ThreadObjectIface::deploy(ThreadObjectIface* parent, ThreadObjectCallbackFun done, void* done_arg)
{
#ifdef R64FX_DEBUG
    assert(!isPending());
    assert(!isDeployed());
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
        else
        {
            parent->m_children.append(this);
            m_parent = parent;
        }
    }
    else
    {
        auto iface_to_impl = new CircularBuffer<ThreadObjectMessage>(32);
        auto impl_to_iface = new CircularBuffer<ThreadObjectMessage>(32);
        
        m_manager = new ThreadObjectManagerIface(iface_to_impl, impl_to_iface);
        m_manager->runThread(newExecAgent());
        
        m_flags |= R64FX_THREAD_OBJECT_IS_ROOT;
    }

    if(m_manager)
    {
        m_manager->deployObject(parent, this, done, done_arg);
        m_flags |= R64FX_THREAD_OBJECT_PENDING;
    }
}


void ThreadObjectIface::withdraw(ThreadObjectCallbackFun done, void* done_arg)
{
#ifdef R64FX_DEBUG
    assert(m_parent != nullptr);
#endif//R64FX_DEBUG

    if(isDeployed())
    {
        m_flags |= R64FX_THREAD_OBJECT_PENDING;
        m_manager->withdrawObject(m_parent, this, done, done_arg);
    }
    else
    {
        m_parent->m_children.remove(this);
        m_parent = nullptr;
    }
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


inline void ThreadObjectIface::sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(m_manager != nullptr);
    assert(m_deployed_impl != nullptr);
#endif//R64FX_DEBUG
    m_manager->sendMessagesToImpl(m_deployed_impl, msgs, nmsgs);
}


inline void ThreadObjectManagerIface::runThread(ThreadObjectExecAgent* agent)
{
    struct Args{
        ThreadObjectExecAgent*                agent;
        CircularBuffer<ThreadObjectMessage>*  iface_to_impl;
        CircularBuffer<ThreadObjectMessage>*  impl_to_iface;
    };
    auto args = new Args;
    args->agent          =  agent;
    args->iface_to_impl  =  m_to_impl;
    args->impl_to_iface  =  m_from_impl;

    m_thread->run([](void* arg) -> void* {
        auto args = (Args*) arg;
        auto agent = args->agent;
        auto manager = new ThreadObjectManagerImpl(args->impl_to_iface, args->iface_to_impl);
        delete args;
        agent->m_manager_impl = manager;
        agent->exec();
        delete manager;
        return nullptr;
    }, args);

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
        else if(m_dst_iface == nullptr)
        {
            dispatchMessageFromImpl(msg);
        }
        else
        {
            m_dst_iface->messageFromImplRecieved(msg);
        }
    }
}


void ThreadObjectManagerIface::deployObject(
    ThreadObjectIface* parent, ThreadObjectIface* child,
    ThreadObjectCallbackFun done, void* done_arg,
    ThreadObjectDeploymentAgent* parent_agent
)
{
    auto agent = child->newDeploymentAgent();
#ifdef R64FX_DEBUG
    assert(agent != nullptr);
#endif//R64FX_DEBUG
    agent->parent_iface = parent;
    agent->child_iface  = child;
    agent->done         = done;
    agent->done_arg     = done_arg;
    agent->parent_agent = parent_agent;

    ThreadObjectMessage msg(DeployObject, agent);
    sendMessagesToImpl(nullptr, &msg, 1);
}


void ThreadObjectManagerIface::objectDeployed(ThreadObjectDeploymentAgent* agent)
{
    auto child =  agent->child_iface;
    auto parent = agent->parent_iface;
    if(parent)
    {
        parent->m_children.append(child);
        child->m_parent = parent;
    }
    child->m_deployed_impl = agent->deployed_impl;
    child->m_flags |= R64FX_THREAD_OBJECT_DEPLOYED;
    child->m_flags &= ~R64FX_THREAD_OBJECT_PENDING;
    if(agent->done)
    {
        agent->done(child, agent->done_arg);
    }
    child->deleteDeploymentAgent(agent);
}


void ThreadObjectManagerIface::withdrawObject(
    ThreadObjectIface* parent, ThreadObjectIface* child,
    ThreadObjectCallbackFun done, void* done_arg,
    ThreadObjectWithdrawalAgent* parent_agent
)
{
    auto agent = child->newWithdrawalAgent();
#ifdef R64FX_DEBUG
    assert(agent != nullptr);
#endif//R64FX_DEBUG
    agent->parent_iface    =  parent;
    agent->public_iface    =  child;
    agent->withdrawn_impl  =  child->m_deployed_impl;

    ThreadObjectMessage msg(WithdrawObject, agent);
    sendMessagesToImpl(nullptr, &msg, 1);
}


void ThreadObjectManagerIface::objectWithdrawn(ThreadObjectWithdrawalAgent* agent)
{
    auto child = agent->public_iface;
    auto parent = agent->parent_iface;
    if(parent)
    {
        parent->m_children.remove(child);
    }
    child->m_parent = nullptr;
    child->m_deployed_impl = nullptr;
    child->m_flags &= ~R64FX_THREAD_OBJECT_DEPLOYED;
    child->m_flags &= ~R64FX_THREAD_OBJECT_PENDING;
    child->deleteWithdrawalAgent(agent);
}


inline void ThreadObjectManagerIface::dispatchMessageFromImpl(const ThreadObjectMessage &msg)
{
    if(msg.key() == ObjectDeployed)
    {
        objectDeployed((ThreadObjectDeploymentAgent*) msg.value());
    }
    else if(msg.key() == ObjectWithdrawn)
    {
        objectWithdrawn((ThreadObjectWithdrawalAgent*) msg.value());
    }
    else
    {
#ifdef R64FX_DEBUG
        cerr << "ThreadObjectManagerIface: Bad Message!\n";
        abort();
#endif//R64FX_DEBUG
    }
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
        agent->deployed_impl = agent->deployImpl(agent->child_iface);
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
    else
    {
#ifdef R64FX_DEBUG
        cerr << "ThreadObjectManagerImpl: Bad Message!\n";
        abort();
#endif//R64FX_DEBUG
    }
}

}//namespace r64fx
