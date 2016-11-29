#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "ThreadObjectFlags.hpp"
#include "ThreadObjectMessage.hpp"
#include "CircularBuffer.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
using std::cerr;
#endif//R64FX_DEBUG

namespace r64fx{

namespace{

enum{
    DeployChild      = 0xFFFFFFFFFFFFFFFB,
    ChildDeployed    = 0xFFFFFFFFFFFFFFFC,
    WithdrawChild    = 0xFFFFFFFFFFFFFFFD,
    ChildWithdrawn   = 0xFFFFFFFFFFFFFFFE,

    PickDestination  = 0xFFFFFFFFFFFFFFFF
};

}


class ThreadObjectCommLink_IfaceEnd{
    CircularBuffer<ThreadObjectMessage>* m_to_impl   = nullptr;
    CircularBuffer<ThreadObjectMessage>* m_from_impl = nullptr;
    ThreadObjectImpl*  m_dst_impl  = nullptr;
    ThreadObjectIface* m_dst_iface = nullptr;

public:
    ThreadObjectCommLink_IfaceEnd(CircularBuffer<ThreadObjectMessage>* to_impl, CircularBuffer<ThreadObjectMessage>* from_impl)
    : m_to_impl(to_impl)
    , m_from_impl(from_impl)
    {
        
    }

    inline void sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs)
    {
        if(m_dst_impl != dst_impl)
        {
            ThreadObjectMessage msg(PickDestination, dst_impl);
            m_dst_impl = dst_impl;
            m_to_impl->write(&msg, 1);
        }
        m_to_impl->write(msgs, nmsgs);
    }

    void readMessagesFromImpl()
    {
        ThreadObjectMessage msg;
        while(m_from_impl->read(&msg, 1))
        {
            if(msg.key() == PickDestination)
            {
                m_dst_iface = (ThreadObjectIface*) msg.value();
            }
            else if(m_dst_impl == nullptr)
            {
                m_dst_iface->dispatchMessage(msg);
            }
            else
            {
                m_dst_iface->messageFromImplRecieved(msg);
            }
        }
    }
};


class ThreadObjectCommLink_ImplEnd{
    CircularBuffer<ThreadObjectMessage>* m_to_iface   = nullptr;
    CircularBuffer<ThreadObjectMessage>* m_from_iface = nullptr;
    ThreadObjectIface* m_dst_iface = nullptr;
    ThreadObjectImpl*  m_dst_impl  = nullptr;

public:
    ThreadObjectCommLink_ImplEnd(CircularBuffer<ThreadObjectMessage>* to_iface, CircularBuffer<ThreadObjectMessage>* from_iface)
    : m_to_iface(to_iface)
    , m_from_iface(from_iface)
    {
        
    }

    inline void sendMessagesToIface(ThreadObjectIface* dst_iface, ThreadObjectMessage* msgs, int nmsgs)
    {
        if(m_dst_iface != dst_iface)
        {
            ThreadObjectMessage msg(PickDestination, dst_iface);
            m_to_iface->write(&msg, 1);
            m_dst_iface = dst_iface;
        }
        m_to_iface->write(msgs, nmsgs);
    }

    void readMessagesFromIface()
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
                m_dst_impl->dispatchMessage(msg);
            }
            else
            {
                m_dst_impl->messageFromIfaceRecieved(msg);
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

    auto agent = child->newDeploymentAgent();
#ifdef R64FX_DEBUG
    assert(agent != nullptr);
#endif//R64FX_DEBUG
    agent->parent_iface = this;
    agent->public_iface = child;

    ThreadObjectMessage msg(DeployChild, agent);
    sendMessagesToImpl(&msg, 1);
    
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
    
    auto agent = child->newWithdrawalAgent();
#ifdef R64FX_DEBUG
    assert(agent != nullptr);
#endif//R64FX_DEBUG
    agent->parent_iface    =  this;
    agent->public_iface    =  child;
    agent->withdrawn_impl  =  child->m_deployed_impl;

    ThreadObjectMessage msg(WithdrawChild, agent);
    sendMessagesToImpl(&msg, 1);
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


inline bool ThreadObjectIface::deploymentPending() const
{
    return !isDeployed() && (m_flags & R64FX_THREAD_OBJECT_PENDING);
}


inline bool ThreadObjectIface::withdrawalPending() const
{
    return isDeployed() && (m_flags & R64FX_THREAD_OBJECT_PENDING);
}


inline void ThreadObjectIface::sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(m_deployed_impl != nullptr);
#endif//R64FX_DEBUG
    m_comm_link->sendMessagesToImpl(m_deployed_impl, msgs, nmsgs);
}


void ThreadObjectIface::dispatchMessage(const ThreadObjectMessage &msg)
{
    switch(msg.key())
    {
        if(msg.key() == ChildDeployed)
        {
            auto agent = (ThreadObjectDeploymentAgent*) msg.value();
            auto child = agent->public_iface;
            m_children.append(child);
            child->m_parent         =  agent->parent_iface;
            child->m_deployed_impl  =  agent->deployed_impl;
            child->deleteDeploymentAgent(agent);
        }
        else if(msg.key() == ChildWithdrawn)
        {
            auto agent = (ThreadObjectWithdrawalAgent*) msg.value();
            auto child = agent->public_iface;
            m_children.remove(child);
            child->m_parent         =  nullptr;
            child->m_deployed_impl  =  nullptr;
            child->deleteWithdrawalAgent(agent);
        }
        else
        {
#ifdef R64FX_DEBUG
            cerr << "ThreadObjectIface: Bad Message!\n";
            abort();
#endif//R64FX_DEBUG
        }
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
    m_comm_link->sendMessagesToIface(m_iface, msgs, nmsgs);
}


void ThreadObjectImpl::dispatchMessage(const ThreadObjectMessage &msg)
{
    switch(msg.key())
    {
        if(msg.key() == DeployChild)
        {
            auto agent = (ThreadObjectDeploymentAgent*) msg.value();
            agent->deployed_impl = agent->deployImpl(agent->public_iface, this);
            ThreadObjectMessage response_msg(ChildDeployed, agent);
            sendMessagesToIface(&response_msg, 1);
        }
        else if(msg.key() == WithdrawChild)
        {
            auto agent = (ThreadObjectWithdrawalAgent*) msg.value();
            agent->withdrawImpl(agent->withdrawn_impl);
            ThreadObjectMessage response_msg(ChildWithdrawn, agent);
            sendMessagesToIface(&response_msg, 1);
        }
        else
        {
#ifdef R64FX_DEBUG
            cerr << "readMessagesFromIface(): Bad Message!\n";
            abort();
#endif//R64FX_DEBUG
        }
    }
}

}//namespace r64fx
