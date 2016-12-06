#ifndef R64FX_THREAD_OBJECT_IMPL_HPP
#define R64FX_THREAD_OBJECT_IMPL_HPP

#include "LinkedList.hpp"
#include "ThreadObjectMessage.hpp"

namespace r64fx{

class ThreadObjectManagerImpl;

class ThreadObjectDeploymentAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*  object_iface   = nullptr;
    ThreadObjectImpl*   deployed_impl  = nullptr;

    ThreadObjectDeploymentAgent* parent_agent = nullptr;
    
    ThreadObjectCallbackFun  done_fun  = nullptr;
    void*                    done_arg  = nullptr;

    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface) = 0;

public:
    virtual ~ThreadObjectDeploymentAgent() {}
};


class ThreadObjectWithdrawalAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*  object_iface    = nullptr;
    ThreadObjectImpl*   withdrawn_impl  = nullptr;

    ThreadObjectWithdrawalAgent* parent_agent = nullptr;

    ThreadObjectCallbackFun  done_fun  = nullptr;
    void*                    done_arg  = nullptr;

    virtual void withdrawImpl(ThreadObjectImpl* impl) = 0;

public:
    virtual ~ThreadObjectWithdrawalAgent() {}
};


class ThreadObjectExecAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*       root_iface   = nullptr;
    ThreadObjectManagerImpl* m_manager_impl = nullptr;

    virtual void exec() = 0;

    virtual void terminate() = 0;

public:
    virtual ~ThreadObjectExecAgent() {}
    
protected:
    void readMessagesFromIface();
};

class ThreadObjectImpl : public LinkedList<ThreadObjectImpl>::Node{
    friend class ThreadObjectManagerImpl;
    ThreadObjectManagerImpl*       m_manager  = nullptr;
    ThreadObjectIface*             m_iface    = nullptr;

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
