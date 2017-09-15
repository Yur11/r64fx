#ifndef R64FX_THREAD_OBJECT_HPP
#define R64FX_THREAD_OBJECT_HPP

/*
 * Framework for objects that have a public interface in the main thread
 * but have their implementations in a separate worker thread.
 */

#include "LinkedList.hpp"
#include "IteratorPair.hpp"
#include "MemoryUtils.hpp"

#define R64FX_DEF_THREAD_OBJECT_IMPL_ARGS ThreadObjectIfaceHandle* iface_handle, ThreadObjectManagerImpl* manager_impl
#define R64FX_THREAD_OBJECT_IMPL_ARGS iface_handle, manager_impl

namespace r64fx{

/*
 * Each ThreadObject consists of a public ThreadObjectIface instance
 * and of a ThreadObjectImpl instance that is deployed in a separate working thread as needed.
 */
class ThreadObjectIface;
class ThreadObjectImpl;

/*
 * Respective ThreadObjectIface & ThreadObjectImpl instances communicate with each other
 * using ThreadObjectMessage instances.
 */
class ThreadObjectMessage;

/*
 * Agents are objects that are created and destroyed in the main thread
 * but perform a specific task inside a worker thread.
 * ThreadObjectIface subclasses must provide two agents.
 * One for deployment and one for withdrawal.
 */
class ThreadObjectDeploymentAgent;
class ThreadObjectWithdrawalAgent;

/*
 * To prevent erronious direct access from a wrong thread these ...Handle classes are used.
 * No definitions for these. Using them just to store pointers.
 */
class ThreadObjectIfaceHandle;
class ThreadObjectImplHandle;

/*  */
class ThreadObjectManagerIface;
class ThreadObjectManagerImpl;


typedef void (*ThreadObjectCallbackFun)(ThreadObjectIface* iface, void* arg);
typedef LinkedList<ThreadObjectIface>::Iterator ThreadObjectIfaceIterator;


/*
 * === Iface ==================================================================
 *
 * ThreadObjectIface instances form a tree hierarchy.
 * Each separate tree uses its own worker thread.
 * Instances belonging to the same tree share their communication link.
 */

class ThreadObjectIface : public LinkedList<ThreadObjectIface>::Node{
    friend class ThreadObjectManagerIface;
    ThreadObjectManagerIface*       m_manager      = nullptr;
    ThreadObjectImplHandle*         m_impl_handle  = nullptr;
    ThreadObjectIface*              m_parent       = nullptr;
    LinkedList<ThreadObjectIface>   m_children;

protected:
    unsigned long m_flags = 0;

public:
    ThreadObjectIface();

    virtual ~ThreadObjectIface();

    /* Deploy ThreadObjectImpl instance in worker thread.
     *
     * This method behaves differently depending on the value of parent.
     * If parent is nullptr this object is deployed in a new thread.
     * If parent is deployed this object is deployed at the thread of the parent.
     * Any children of this object are recursivly deployed as well.
     * The done_fun callback is called once the deployemnt procedure is completed.
     * The deploy() method itself returns immediately.
     * If parent is not deployed this object is simply added to the tree to be later deployed with it.
     * The callback is not called in that case.
     *
     * Calling this method on an object that is already deployed shall fail.
     */
    void deploy(ThreadObjectIface* parent = nullptr, ThreadObjectCallbackFun done_fun = nullptr, void* done_arg = nullptr);

    /* Withdraw ThreadObjectImpl instance from the worker thread.
     *
     * Any children of this object are recursivly withdrawn before their parents.
     *
     * The done_fun callback is called once the withdrawal procedure is completed.
     * The withdraw() method itself returns immediately.
     */
    void withdraw(ThreadObjectCallbackFun done_fun = nullptr, void* done_arg = nullptr);

    ThreadObjectIface* parent() const;

    inline bool empty() const { return m_children.empty(); }

    IteratorPair<ThreadObjectIfaceIterator> children() const;

    bool isDeployed() const;

    bool isPending() const;

    bool deploymentPending() const;

    bool withdrawalPending() const;

protected:
    void sendMessagesToImpl(const ThreadObjectMessage* msgs, int nmsgs);

    inline ThreadObjectImplHandle* impl() const { return m_impl_handle; }

private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) = 0;

    /*
     * These are called during deployemnt and withdrawal procedures.
     */
    virtual ThreadObjectDeploymentAgent* newDeploymentAgent() = 0;

    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent) = 0;

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent() = 0;

    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent) = 0;
};


class ThreadObjectMessage{
    /* Key 0xFFFFFFFFFFFFFFFF (All bits 1) is reserved!*/
    unsigned long m_key     = 0;
    unsigned long m_value   = 0;

public:
    ThreadObjectMessage(unsigned long key, unsigned long value)
    : m_key(key)
    , m_value(value)
    {

    }

    ThreadObjectMessage(unsigned long key, void* value)
    : m_key(key)
    , m_value((unsigned long)value)
    {

    }

    ThreadObjectMessage(unsigned long key)
    : m_key(key)
    , m_value(0)
    {

    }

    ThreadObjectMessage() {}


    inline unsigned long key() const
    {
        return m_key;
    }

    inline unsigned long value() const
    {
        return m_value;
    }
};


/*
 * === Agents =================================================================
 *
 * These are created and destroyed in the main thread but used only in the worker thread.
 * Additional fields shall be added by subclasses with any information needed for deployemnt or withdrawal.
 */

class ThreadObjectDeploymentAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*            object_iface   = nullptr;
    ThreadObjectImplHandle*       deployed_impl  = nullptr;
    ThreadObjectDeploymentAgent*  parent_agent   = nullptr;
    ThreadObjectCallbackFun       done_fun       = nullptr;
    void*                         done_arg       = nullptr;

    /* Create and return a ThreadObjectImpl instance. */
    virtual ThreadObjectImpl* deployImpl(HeapAllocator* ha, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS) = 0;

public:
    virtual ~ThreadObjectDeploymentAgent() {}
};


class ThreadObjectWithdrawalAgent{
    friend class ThreadObjectManagerIface;
    friend class ThreadObjectManagerImpl;

    ThreadObjectIface*       object_iface    = nullptr;
    ThreadObjectImplHandle*  withdrawn_impl  = nullptr;

    ThreadObjectWithdrawalAgent* parent_agent = nullptr;

    ThreadObjectCallbackFun  done_fun  = nullptr;
    void*                    done_arg  = nullptr;

    /* Destroy the ThreadObjectImpl instance.
     * If needed gather and return any additional information to the main thread. */
    virtual void withdrawImpl(HeapAllocator* ha, ThreadObjectImpl* impl) = 0;

public:
    virtual ~ThreadObjectWithdrawalAgent() {}
};


/*
 * === Impl ===================================================================
 *
 * ThreadObjectImpl instances do not form a hierarchy by default.
 * Add that in a subclass if needed.
 */

class ThreadObjectImpl{
    friend class ThreadObjectManagerImpl;
    ThreadObjectIfaceHandle*  m_iface_handle  = nullptr;
    ThreadObjectManagerImpl*  m_manager_impl  = nullptr;

public:
    ThreadObjectImpl(R64FX_DEF_THREAD_OBJECT_IMPL_ARGS);

    virtual ~ThreadObjectImpl();

protected:
    void sendMessagesToIface(const ThreadObjectMessage* msgs, int nmsgs);

    void readMessagesFromIface();

    /*
     * Each worker thread is provided with its own HeapAllocator instance.
     */
    HeapAllocator* heapAllocator() const;

    template<typename T, typename... CtorArgs> inline T* allocObj(CtorArgs... ctor_args)
    {
        return heapAllocator()->allocObj<T, CtorArgs...>(ctor_args...);
    }

    template<typename T> inline void freeObj(T* obj)
    {
        heapAllocator()->freeObj<T>(obj);
    }

    ThreadObjectImpl* rootImpl() const;

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) = 0;

    /* Worker thread loop.
     *
     * readMessagesFromIface() method must be periodically called.
     */
    virtual void runThread() = 0;

    /* Stop and exit the worker thread. */
    virtual void exitThread() = 0;
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_HPP
