#ifndef R64FX_THREAD_OBJECT_IFACE_HPP
#define R64FX_THREAD_OBJECT_IFACE_HPP

#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class ThreadObjectIface;
class ThreadObjectImpl;
class ThreadObjectMessage;
class ThreadObjectManagerIface;
class ThreadObjectManagerImpl;
typedef void (*ThreadObjectCallbackFun)(ThreadObjectIface* iface, void* arg);


class ThreadObjectDeploymentAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*  parent_iface   = nullptr;
    ThreadObjectIface*  public_iface   = nullptr;
    ThreadObjectImpl*   deployed_impl  = nullptr;
    
    ThreadObjectCallbackFun  done      = nullptr;
    void*                    done_arg  = nullptr;

    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface) = 0;

public:
    virtual ~ThreadObjectDeploymentAgent() {}
};


class ThreadObjectWithdrawalAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*  parent_iface    = nullptr;
    ThreadObjectIface*  public_iface    = nullptr;
    ThreadObjectImpl*   withdrawn_impl  = nullptr;

    virtual void withdrawImpl(ThreadObjectImpl* impl) = 0;

public:
    virtual ~ThreadObjectWithdrawalAgent() {}
};


class ThreadObjectExecAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectManagerImpl* m_manager_impl = nullptr;

    virtual void exec() = 0;

public:
    virtual ~ThreadObjectExecAgent() {}
    
protected:
    void readMessagesFromIface();
};


typedef LinkedList<ThreadObjectIface>::Iterator ThreadObjectIfaceIterator;

class ThreadObjectIface : public LinkedList<ThreadObjectIface>::Node{
    friend class ThreadObjectManagerIface;
    ThreadObjectManagerIface*       m_manager        = nullptr;
    ThreadObjectImpl*               m_deployed_impl  = nullptr;
    ThreadObjectIface*              m_parent         = nullptr;
    LinkedList<ThreadObjectIface>   m_children;

protected:
    unsigned int m_flags = 0;

public:
    ThreadObjectIface();

    virtual ~ThreadObjectIface();

    void deploy(ThreadObjectIface* parent = nullptr, ThreadObjectCallbackFun done = nullptr, void* done_arg = nullptr);

    void withdraw(ThreadObjectCallbackFun done = nullptr, void* done_arg = nullptr);

    ThreadObjectIface* parent() const;

    IteratorPair<ThreadObjectIfaceIterator> children() const;

    bool isDeployed() const;

    bool isPending() const;
    
    bool deploymentPending() const;

    bool withdrawalPending() const;

    bool isThreadRoot() const;

protected:
    void sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs);

private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) = 0;

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent() = 0;

    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent) = 0;

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent() = 0;
    
    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent) = 0;
    
    virtual ThreadObjectExecAgent* newExecAgent() = 0;
    
    virtual void deleteExecAgent(ThreadObjectExecAgent* agent) = 0;
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_IFACE_HPP
