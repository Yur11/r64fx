#include "ThreadObjectCommLink.hpp"
#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "ThreadObjectFlags.hpp"

namespace r64fx{

namespace{

enum{
    PickDestination,
    DeployChild,
    ChildDeployed,
    WithdrawChild,
    ChildWithdrawn
};

}
    
inline void ThreadObjectCommLink_IfaceEnd::sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs)
{
    if(dst_impl != m_dst_impl)
    {
        m_dst_impl = m_dst_impl;
        ThreadObjectMessage msg(PickDestination, m_dst_impl);
        m_to_impl->write(&msg, 1);
    }
    m_to_impl->write(msgs, nmsgs);
}


void ThreadObjectCommLink_IfaceEnd::readMessagesFromImpl()
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
            m_dst_iface->messageFromImplRecieved(msg);
        }
    }
}



inline void ThreadObjectCommLink_ImplEnd::sendMessagesToIface(ThreadObjectIface* dst_iface, ThreadObjectMessage* msgs, int nmsgs)
{
    if(dst_iface != m_dst_iface)
    {
        m_dst_iface = dst_iface;
        ThreadObjectMessage msg(PickDestination, m_dst_iface);
        m_to_iface->write(&msg, 1);
    }
    m_to_iface->write(msgs, nmsgs);
}


void ThreadObjectCommLink_ImplEnd::readMessagesFromIface()
{
    ThreadObjectMessage msg;
    while(m_from_iface->read(&msg, 1))
    {
        if(msg.key() == PickDestination)
        {
            m_dst_impl = (ThreadObjectImpl*) msg.value();
        }
        else
        {
            m_dst_impl->messageFromIfaceRecieved(msg);
        }
    }
}



ThreadObjectIface::ThreadObjectIface(ThreadObjectIface* parent_iface)
{
    
}


ThreadObjectIface::~ThreadObjectIface()
{
    
}

void ThreadObjectIface::setParent(ThreadObjectImpl* parent_iface)
{
    
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


void ThreadObjectIface::sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs)
{
    
}


ThreadObjectImpl::ThreadObjectImpl(ThreadObjectIface* iface)
: m_iface(iface)
{
    
}


ThreadObjectImpl::~ThreadObjectImpl()
{
    
}

void ThreadObjectImpl::sendMessagesToIface(ThreadObjectMessage* msgs, int nmsgs)
{
    
}

}//namespace r64fx
