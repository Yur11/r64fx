#ifndef R64FX_THREAD_OBJECT_IMPL_HPP
#define R64FX_THREAD_OBJECT_IMPL_HPP

#include "LinkedList.hpp"

namespace r64fx{

class ThreadObjectManagerImpl;

class ThreadObjectImpl : public LinkedList<ThreadObjectImpl>::Node{
    friend class ThreadObjectManagerImpl;
    ThreadObjectManagerImpl*       m_manager  = nullptr;
    ThreadObjectIface*             m_iface    = nullptr;
    ThreadObjectImpl*              m_parent   = nullptr;
    LinkedList<ThreadObjectImpl>   m_children;

public:
    ThreadObjectImpl(ThreadObjectIface* iface);

    virtual ~ThreadObjectImpl();

protected:
    void sendMessagesToIface(ThreadObjectMessage* msgs, int nmsgs);

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_IMPL_HPP
