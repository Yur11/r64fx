#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "ThreadObjectFlags.hpp"
#include "ThreadObjectMessage.hpp"
#include "CircularBuffer.hpp"
#include "Thread.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
using std::cerr;
#endif//R64FX_DEBUG

namespace r64fx{

namespace{

enum{
    DeployChild,
    ChildDeployed,
    WithdrawChild,
    ChildWithdrawn,
    TerminateThread,

    PickDestination  = 0xFFFFFFFFFFFFFFFF
};

}


struct ThreadObjectManagerIface{
    Thread* thread = nullptr;

    CircularBuffer<ThreadObjectMessage>* to_impl   = nullptr;
    CircularBuffer<ThreadObjectMessage>* from_impl = nullptr;

    ThreadObjectImpl*  dst_impl  = nullptr;
    ThreadObjectIface* dst_iface = nullptr;

    ThreadObjectManagerIface(CircularBuffer<ThreadObjectMessage>* to_impl, CircularBuffer<ThreadObjectMessage>* from_impl)
    : to_impl(to_impl)
    , from_impl(from_impl)
    {
        
    }

    inline void sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs)
    {
        if(this->dst_impl != dst_impl)
        {
            ThreadObjectMessage msg(PickDestination, dst_impl);
            this->dst_impl = dst_impl;
            to_impl->write(&msg, 1);
        }
        to_impl->write(msgs, nmsgs);
    }

    void readMessagesFromImpl()
    {
        ThreadObjectMessage msg;
        while(from_impl->read(&msg, 1))
        {
            if(msg.key() == PickDestination)
            {
                dst_iface = (ThreadObjectIface*) msg.value();
            }
            else if(dst_impl == nullptr)
            {
                dispatchMessageFromImpl(msg);
            }
            else
            {
                dst_iface->messageFromImplRecieved(msg);
            }
        }
    }

    inline void dispatchMessageFromImpl(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
            if(msg.key() == ChildDeployed)
            {
                auto agent = (ThreadObjectDeploymentAgent*) msg.value();
                auto child = agent->public_iface;
                dst_iface->m_children.append(child);
                child->m_parent = agent->parent_iface;
                child->m_deployed_impl = agent->deployed_impl;
                child->deleteDeploymentAgent(agent);
                child->m_flags |= R64FX_THREAD_OBJECT_DEPLOYED;
                child->m_flags &= ~R64FX_THREAD_OBJECT_PENDING;
            }
            else if(msg.key() == ChildWithdrawn)
            {
                auto agent = (ThreadObjectWithdrawalAgent*) msg.value();
                auto child = agent->public_iface;
                dst_iface->m_children.remove(child);
                child->m_parent = nullptr;
                child->m_deployed_impl = nullptr;
                child->deleteWithdrawalAgent(agent);
                child->m_flags &= ~R64FX_THREAD_OBJECT_DEPLOYED;
                child->m_flags &= ~R64FX_THREAD_OBJECT_PENDING;
            }
            else
            {
#ifdef R64FX_DEBUG
                cerr << "ThreadObjectManagerIface: Bad Message!\n";
                abort();
#endif//R64FX_DEBUG
            }
        }
    }

    inline void deployObject(ThreadObjectIface* parent, ThreadObjectIface* child)
    {
        auto agent = child->newDeploymentAgent();
#ifdef R64FX_DEBUG
        assert(agent != nullptr);
#endif//R64FX_DEBUG
        agent->parent_iface = parent;
        agent->public_iface = child;

        ThreadObjectMessage msg(DeployChild, agent);
        sendMessagesToImpl(nullptr, &msg, 1);
    }

    inline void withdrawObject(ThreadObjectIface* parent, ThreadObjectIface* child)
    {
        auto agent = child->newWithdrawalAgent();
#ifdef R64FX_DEBUG
        assert(agent != nullptr);
#endif//R64FX_DEBUG
        agent->parent_iface    =  parent;
        agent->public_iface    =  child;
        agent->withdrawn_impl  =  child->m_deployed_impl;

        ThreadObjectMessage msg(WithdrawChild, agent);
        sendMessagesToImpl(nullptr, &msg, 1);
    }
};


struct ThreadObjectManagerImpl{
    CircularBuffer<ThreadObjectMessage>* to_iface   = nullptr;
    CircularBuffer<ThreadObjectMessage>* from_iface = nullptr;
    ThreadObjectIface* dst_iface = nullptr;
    ThreadObjectImpl*  dst_impl  = nullptr;

    ThreadObjectManagerImpl(CircularBuffer<ThreadObjectMessage>* to_iface, CircularBuffer<ThreadObjectMessage>* from_iface)
    : to_iface(to_iface)
    , from_iface(from_iface)
    {
        
    }

    inline void sendMessagesToIface(ThreadObjectIface* dst_iface, ThreadObjectMessage* msgs, int nmsgs)
    {
        if(this->dst_iface != dst_iface)
        {
            ThreadObjectMessage msg(PickDestination, dst_iface);
            to_iface->write(&msg, 1);
            this->dst_iface = dst_iface;
        }
        to_iface->write(msgs, nmsgs);
    }

    void readMessagesFromIface()
    {
        ThreadObjectMessage msg;
        while(from_iface->read(&msg, 1))
        {
            if(msg.key() == PickDestination)
            {
                dst_impl = (ThreadObjectImpl*) msg.value();
            }
            else if(dst_impl == nullptr)
            {
                dispatchMessageFromIface(msg);
            }
            else
            {
                dst_impl->messageFromIfaceRecieved(msg);
            }
        }
    }

    inline void dispatchMessageFromIface(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
            if(msg.key() == DeployChild)
            {
                auto agent = (ThreadObjectDeploymentAgent*) msg.value();
                agent->deployed_impl = agent->deployImpl(agent->public_iface, nullptr);
                ThreadObjectMessage response_msg(ChildDeployed, agent);
                sendMessagesToIface(nullptr, &response_msg, 1);
            }
            else if(msg.key() == WithdrawChild)
            {
                auto agent = (ThreadObjectWithdrawalAgent*) msg.value();
                agent->withdrawImpl(agent->withdrawn_impl);
                ThreadObjectMessage response_msg(ChildWithdrawn, agent);
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
    }
};


ThreadObjectIface::ThreadObjectIface(ThreadObjectIface* parent_iface)
{
    
}


ThreadObjectIface::~ThreadObjectIface()
{
    
}


inline ThreadObjectIface* ThreadObjectIface::parent() const
{
    return m_parent;
}


inline IteratorPair<ThreadObjectIfaceIterator> ThreadObjectIface::children() const
{
    return {m_children.begin(), m_children.end()};
}


void ThreadObjectIface::deployChild(ThreadObjectIface* child)
{
#ifdef R64FX_DEBUG
    assert(!isDeployed());
#endif//R64FX_DEBUG
    m_manager->deployObject(this, child);
    m_flags |= R64FX_THREAD_OBJECT_PENDING;
}


void ThreadObjectIface::withdrawChild(ThreadObjectIface* child)
{
#ifdef R64FX_DEBUG
    assert(child->parent() == this);
    assert(isDeployed());
    assert(child->isDeployed());
#endif//R64FX_DEBUG

    child->withdrawAllChildren();
    m_manager->withdrawObject(this, child);
}


void ThreadObjectIface::withdrawAllChildren()
{
    for(auto child : children())
    {
        withdrawChild(child);
    }
}


void ThreadObjectIface::withdrawFromParent()
{
#ifdef R64FX_DEBUG
    assert(parent() != nullptr);
#endif//R64FX_DEBUG

    parent()->withdrawChild(this);
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


void ThreadObjectIface::deployThreadRoot()
{
#ifdef R64FX_DEBUG
    assert(!isThreadRoot());
    assert(!isDeployed());
    assert(!isPending());
#endif//R64FX_DEBUG

    struct Args{
        ThreadObjectDeploymentAgent*          deployment_agent;
        ThreadObjectThreadExecAgent*          exec_agent;
        CircularBuffer<ThreadObjectMessage>*  iface_to_impl;
        CircularBuffer<ThreadObjectMessage>*  impl_to_iface;
    };
    auto args = new Args;
    args->deployment_agent  =  newDeploymentAgent();
    args->exec_agent        =  newExecAgent();
    args->iface_to_impl     =  new CircularBuffer<ThreadObjectMessage>(32);
    args->impl_to_iface     =  new CircularBuffer<ThreadObjectMessage>(32);
    
    m_manager               =  new ThreadObjectManagerIface(args->iface_to_impl, args->impl_to_iface);
    m_manager->thread       =  new Thread;
    
    m_flags |= (R64FX_THREAD_OBJECT_IS_ROOT & R64FX_THREAD_OBJECT_PENDING);
}


void ThreadObjectIface::withdrawThreadRoot()
{
    
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

}//namespace r64fx
