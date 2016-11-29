#ifndef R64FX_THREAD_OBJECT_IMPL_HPP
#define R64FX_THREAD_OBJECT_IMPL_HPP

#include "LinkedList.hpp"

namespace r64fx{

class ThreadObjectCommLink_ImplEnd;

class ThreadObjectImpl : public LinkedList<ThreadObjectImpl>::Node{
    friend class ThreadObjectCommLink_ImplEnd;
    ThreadObjectCommLink_ImplEnd*  m_comm_link     = nullptr;
    ThreadObjectIface*             m_iface  = nullptr;
    ThreadObjectImpl*              m_parent;
    LinkedList<ThreadObjectImpl>   m_children;

public:
    ThreadObjectImpl(ThreadObjectIface* iface);

    virtual ~ThreadObjectImpl();

protected:
    void sendMessagesToIface(ThreadObjectMessage* msgs, int nmsgs);

private:
    void dispatchMessage(const ThreadObjectMessage &msg);

    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_IMPL_HPP
