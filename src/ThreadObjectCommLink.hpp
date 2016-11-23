#ifndef R64FX_THREAD_OBJECT_COMM_LINK_HPP
#define R64FX_THREAD_OBJECT_COMM_LINK_HPP

#include "CircularBuffer.hpp"
#include "ThreadObjectMessage.hpp"

namespace r64fx{

class ThreadObjectIface;
class ThreadObjectImpl;

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

    void sendMessagesToImpl(ThreadObjectImpl* dst_impl, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromImpl();
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

    void sendMessagesToIface(ThreadObjectIface* dst_iface, ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromIface();
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_COMM_LINK_HPP
