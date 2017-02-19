#include "ThreadObject.hpp"
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

    /* Reserved. Must not be used by derived classes. */
    PickDestination  = 0xFFFFFFFFFFFFFFFF
};

}//namespace


class ThreadObjectManagerIface{
    Thread*                               m_thread           = nullptr;
    Timer*                                m_timer            = nullptr;
    CircularBuffer<ThreadObjectMessage>*  m_to_impl          = nullptr;
    CircularBuffer<ThreadObjectMessage>*  m_from_impl        = nullptr;
    ThreadObjectImplHandle*               m_dst_impl_handle  = nullptr;
    ThreadObjectIface*                    m_dst_iface        = nullptr;
    ThreadObjectIface*                    m_root_iface       = nullptr;

public:
    void createThreadAndDeployRoot(
        ThreadObjectIface* root_object,
        ThreadObjectCallbackFun done, void* done_arg
    );

    void deployObject(
        ThreadObjectIface* object,
        ThreadObjectCallbackFun done, void* done_arg,
        ThreadObjectDeploymentAgent* parent_agent
    );

    void objectDeployed(ThreadObjectDeploymentAgent* agent);

    void sendMessagesToImpl(ThreadObjectImplHandle* dst_impl, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromImpl();

    void withdrawObject(
        ThreadObjectIface* object,
        ThreadObjectCallbackFun done, void* done_arg,
        ThreadObjectWithdrawalAgent* parent_agent
    );

    void objectWithdrawn(ThreadObjectWithdrawalAgent* agent);

    void dispatchMessageFromImpl(const ThreadObjectMessage &msg);

    /* Don't touch the object after calling! */
    void suicide();

    /* Set or clear a group of flags of a ThreadObjectIface subtree. */
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


class ThreadObjectManagerImpl{
    CircularBuffer<ThreadObjectMessage>*  m_to_iface          = nullptr;
    CircularBuffer<ThreadObjectMessage>*  m_from_iface        = nullptr;
    ThreadObjectIfaceHandle*              m_dst_iface         = nullptr;
    ThreadObjectImpl*                     m_dst_impl          = nullptr;
    ThreadObjectImpl*                     m_root_impl         = nullptr;
    HeapAllocator*                        m_heap_allocator    = nullptr;
    ThreadObjectWithdrawalAgent*          m_withdrawal_agent  = nullptr;
    void*                                 m_asset             = nullptr;

public:
    ThreadObjectManagerImpl(
        CircularBuffer<ThreadObjectMessage>* to_iface,
        CircularBuffer<ThreadObjectMessage>* from_iface,
        HeapAllocator* heap_allocator
    )
    : m_to_iface(to_iface)
    , m_from_iface(from_iface)
    , m_heap_allocator(heap_allocator)
    {

    }

    void execThread(ThreadObjectDeploymentAgent* agent);

    inline ThreadObjectImpl* rootImpl() const
    {
        return m_root_impl;
    }

    inline HeapAllocator* heapAllocator() const
    {
        return m_heap_allocator;
    }

    inline void setAsset(void* asset)
    {
        m_asset = asset;
    }

    inline void* asset() const
    {
        return m_asset;
    }

    void sendMessagesToIface(ThreadObjectIfaceHandle* dst_iface, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromIface();

    void dispatchMessageFromIface(const ThreadObjectMessage &msg);

    ThreadObjectImpl* doDeployObject(ThreadObjectDeploymentAgent* agent);

    void doWithdrawObject(ThreadObjectWithdrawalAgent* agent);
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
#else
    if(isPending() || isDeployed() || m_parent != nullptr)
        return;
#endif//R64FX_DEBUG

    if(parent)
    {
#ifdef R64FX_DEBUG
        assert(!parent->isPending());
#else
        if(parent->isPending())
            return;
#endif//R64FX_DEBUG
        parent->m_children.append(this);
        m_parent = parent;
        if(parent->isDeployed())
        {
            m_manager = parent->m_manager;
            ThreadObjectManagerIface::alterTreeFlags(this, R64FX_THREAD_OBJECT_PENDING, true);
            m_manager->deployObject(this, done_fun, done_arg, nullptr);
        }
    }
    else
    {
        m_manager = new ThreadObjectManagerIface;
        ThreadObjectManagerIface::alterTreeFlags(this, R64FX_THREAD_OBJECT_PENDING, true);
        m_manager->createThreadAndDeployRoot(this, done_fun, done_arg);
    }
}


void ThreadObjectIface::withdraw(ThreadObjectCallbackFun done_fun, void* done_arg)
{
    if(isDeployed())
    {
        m_flags |= R64FX_THREAD_OBJECT_IS_WITHDRAWAL_ROOT;
        ThreadObjectManagerIface::alterTreeFlags(this, R64FX_THREAD_OBJECT_PENDING, true);
        m_manager->withdrawObject(this, done_fun, done_arg, nullptr);
    }
    else
    {
        m_parent->m_children.remove(this);
        m_parent = nullptr;
    }
}


ThreadObjectIface* ThreadObjectIface::parent() const
{
    return m_parent;
}


IteratorPair<ThreadObjectIfaceIterator> ThreadObjectIface::children() const
{
    return {m_children.begin(), m_children.end()};
}


bool ThreadObjectIface::isDeployed() const
{
    return m_flags & R64FX_THREAD_OBJECT_DEPLOYED;
}


bool ThreadObjectIface::isPending() const
{
    return m_flags & R64FX_THREAD_OBJECT_PENDING;
}


bool ThreadObjectIface::deploymentPending() const
{
    return (!isDeployed()) && isPending();
}


bool ThreadObjectIface::withdrawalPending() const
{
    return isDeployed() && isPending();
}


void ThreadObjectIface::sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(m_manager != nullptr);
    assert(m_impl_handle != nullptr);
#endif//R64FX_DEBUG
    m_manager->sendMessagesToImpl(m_impl_handle, msgs, nmsgs);
}


inline void ThreadObjectManagerIface::createThreadAndDeployRoot(
    ThreadObjectIface* root_object_iface,
    ThreadObjectCallbackFun done, void* done_arg
)
{
    m_root_iface = root_object_iface;
    m_root_iface->m_manager = this;

    auto agent = m_root_iface->newDeploymentAgent();
    agent->object_iface  = m_root_iface;
    agent->done_fun      = done;
    agent->done_arg      = done_arg;
    agent->parent_agent  = nullptr;

    auto iface_to_impl  =  m_to_impl    = new CircularBuffer<ThreadObjectMessage>(32);
    auto impl_to_iface  =  m_from_impl  = new CircularBuffer<ThreadObjectMessage>(32);
    ThreadObjectMessage argmsg((unsigned long) impl_to_iface, (unsigned long) agent);
    iface_to_impl->write(&argmsg, 1);

    m_thread = new Thread;
    m_thread->run([](void* arg) -> void* {
        auto iface_to_impl = (CircularBuffer<ThreadObjectMessage>*) arg;
        ThreadObjectMessage argmsg;
        iface_to_impl->read(&argmsg, 1);
        auto impl_to_iface = (CircularBuffer<ThreadObjectMessage>*) argmsg.key();
        auto agent = (ThreadObjectDeploymentAgent*) argmsg.value();

        HeapAllocator ha;
        auto manager_impl = ha.allocObj<ThreadObjectManagerImpl>(impl_to_iface, iface_to_impl, &ha);

        manager_impl->execThread(agent);

        ha.freeObj(manager_impl);
#ifdef R64FX_DEBUG
        assert(ha.isEmpty());
#endif//R64FX_DEBUG
        return nullptr;
    }, iface_to_impl);

    m_timer = new Timer;
    m_timer->onTimeout([](Timer* timer, void* arg){
        auto self = (ThreadObjectManagerIface*) arg;
        self->readMessagesFromImpl();
    }, this);
    m_timer->setInterval(5000 * 1000);
    m_timer->start();
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
    object->m_impl_handle = agent->deployed_impl;
    if(object->m_children.isEmpty())
    {
        auto parent_agent = agent->parent_agent;
        for(;;)
        {
            if(parent_agent)
            {
                object->deleteDeploymentAgent(agent);
                auto sibling = object->next();
                if(sibling)
                {
                    deployObject(sibling, nullptr, nullptr, parent_agent);
                    break;
                }
                else
                {
                    object = parent_agent->object_iface;
                    agent = parent_agent;
                    parent_agent = parent_agent->parent_agent;
                    continue;
                }
            }
            else
            {
                done_fun = agent->done_fun;
                done_arg = agent->done_arg;
                done = true;
                object->deleteDeploymentAgent(agent);
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


inline void ThreadObjectManagerIface::sendMessagesToImpl(ThreadObjectImplHandle* dst_impl, ThreadObjectMessage* msgs, int nmsgs)
{
    if(m_dst_impl_handle != dst_impl)
    {
        ThreadObjectMessage msg(PickDestination, dst_impl);
        m_dst_impl_handle = dst_impl;
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
            }
            else
            {
                m_dst_iface->messageFromImplRecieved(msg);
            }
        }
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
        agent->withdrawn_impl  =  object->m_impl_handle;
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

        auto done_fun = agent->done_fun;
        auto done_arg = agent->done_arg;
        object->deleteWithdrawalAgent(agent);
        if(done_fun)
        {
            done_fun(object, done_arg);
        }
    }

    object->m_manager = nullptr;
    if(object->m_parent)
    {
        if(object->m_flags & R64FX_THREAD_OBJECT_IS_WITHDRAWAL_ROOT)
        {
            object->m_parent->m_children.remove(object);
            object->m_parent = nullptr;
            object->m_flags &= ~R64FX_THREAD_OBJECT_IS_WITHDRAWAL_ROOT;
        }
    }
    else
    {
        /* Initiate Suicide */
        m_timer->onTimeout([](Timer* timer, void* arg){
            auto self = (ThreadObjectManagerIface*) arg;
            self->suicide();
        }, this);
        m_timer->setInterval(0);
    }
}


void ThreadObjectManagerIface::suicide()
{
    m_timer->stop();
    m_timer->suicide();
    m_thread->join();
    delete m_thread;
    delete m_to_impl;
    delete m_from_impl;
    delete this;
}


ThreadObjectImpl::ThreadObjectImpl(ThreadObjectIfaceHandle* iface_handle, ThreadObjectManagerImpl* manager_impl)
: m_iface_handle(iface_handle)
, m_manager_impl(manager_impl)
{

}


ThreadObjectImpl::~ThreadObjectImpl()
{

}


void ThreadObjectImpl::sendMessagesToIface(ThreadObjectMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(m_iface_handle != nullptr);
#endif//R64FX_DEBUG
    m_manager_impl->sendMessagesToIface(m_iface_handle, msgs, nmsgs);
}


void ThreadObjectImpl::readMessagesFromIface()
{
    m_manager_impl->readMessagesFromIface();
}


HeapAllocator* ThreadObjectImpl::heapAllocator() const
{
    return m_manager_impl->heapAllocator();
}


void ThreadObjectImpl::setAsset(void* asset)
{
    m_manager_impl->setAsset(asset);
}


void* ThreadObjectImpl::asset() const
{
    return m_manager_impl->asset();
}


void ThreadObjectManagerImpl::execThread(ThreadObjectDeploymentAgent* agent)
{
    m_root_impl = doDeployObject(agent);
    m_root_impl->runThread();
    if(m_withdrawal_agent)
    {
        doWithdrawObject(m_withdrawal_agent);
    }
}


inline void ThreadObjectManagerImpl::sendMessagesToIface(ThreadObjectIfaceHandle* dst_iface, ThreadObjectMessage* msgs, int nmsgs)
{
    if(m_dst_iface != dst_iface)
    {
        ThreadObjectMessage msg(PickDestination, dst_iface);
        m_to_iface->write(&msg, 1);
        m_dst_iface = dst_iface;
    }
    m_to_iface->write(msgs, nmsgs);
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
    switch(msg.key())
    {
        case DeployObject:
        {
            doDeployObject((ThreadObjectDeploymentAgent*) msg.value());
            break;
        }

        case WithdrawObject:
        {
            auto agent = (ThreadObjectWithdrawalAgent*) msg.value();
            auto withdrawn_impl = (ThreadObjectImpl*) agent->withdrawn_impl;
            if(withdrawn_impl == m_root_impl)
            {
                m_withdrawal_agent = agent;
                withdrawn_impl->exitThread();
            }
            else
            {
                doWithdrawObject(agent);
            }
            break;
        }

        default:
        {
#ifdef R64FX_DEBUG
            cerr << "ThreadObjectManagerImpl: Bad Message!\n";
            abort();
#endif//R64FX_DEBUG
            break;
        }
    }
}


ThreadObjectImpl* ThreadObjectManagerImpl::doDeployObject(ThreadObjectDeploymentAgent* agent)
{
    auto impl = agent->deployImpl(m_heap_allocator, (ThreadObjectIfaceHandle*)agent->object_iface, this);
    agent->deployed_impl = (ThreadObjectImplHandle*) impl;
    ThreadObjectMessage response_msg(ObjectDeployed, agent);
    sendMessagesToIface(nullptr, &response_msg, 1);
    return impl;
}


void ThreadObjectManagerImpl::doWithdrawObject(ThreadObjectWithdrawalAgent* agent)
{
    agent->withdrawImpl(m_heap_allocator, (ThreadObjectImpl*)agent->withdrawn_impl);
    ThreadObjectMessage response_msg(ObjectWithdrawn, agent);
    sendMessagesToIface(nullptr, &response_msg, 1);
}

}//namespace r64fx
