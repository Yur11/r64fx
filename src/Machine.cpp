#include "Machine.hpp"
#include "MachineImpl.hpp"
#include "MachineManager.hpp"
#include "Thread.hpp"
#include "Timer.hpp"
#include "CircularBuffer.hpp"

namespace r64fx{

namespace{
    constexpr unsigned long PickDestination = 0;
    
    constexpr unsigned long Terminate       = 1;
    constexpr unsigned long Deploy          = 2;
    constexpr unsigned long Withdraw        = 3;
}//namespace
    

void impl_thread(void* arg)
{
    
}

    
class MachineManagerPrivate{
    Thread*  m_thread = nullptr;
    Timer*   m_timer  = nullptr;
    
    CircularBuffer<MachineMessage>* m_to_impl   = nullptr;
    CircularBuffer<MachineMessage>* m_from_impl = nullptr;
  
    MachineImpl* m_dst_impl  = nullptr;
    Machine*     m_dst_iface = nullptr;
    
public:
    MachineManagerPrivate()
    {
        m_thread     = new Thread;
        m_timer      = new Timer;
        m_to_impl    = new CircularBuffer<MachineMessage>(32);
        m_from_impl  = new CircularBuffer<MachineMessage>(32);
        
        startImplThread();
        
        m_timer->onTimeout([](Timer* timer, void* arg){
            auto self = (MachineManagerPrivate*) arg;
            self->dispatchMessages();
        }, this);
        m_timer->setInterval(500);
        m_timer->start();
    }
    
    ~MachineManagerPrivate()
    {
        m_timer->stop();
        
        stopImplThread();
        
        delete m_thread;
        delete m_timer;
        delete m_to_impl;
        delete m_from_impl;
    }
    
    void sendMessages(const MachineMessage* msgs, int nmsgs)
    {
        m_to_impl->write(msgs, nmsgs);
    }

    void sendMessages(MachineImpl* dst, const MachineMessage* msgs, int nmsgs)
    {
        if(dst != m_dst_impl)
            pickDestination(dst);
        sendMessages(msgs, nmsgs);
    }
    
    void pickDestination(MachineImpl* dst)
    {
        MachineMessage msg(PickDestination, (unsigned long)dst);
        sendMessages(&msg, 1);
        m_dst_impl = dst;
    }
    
    void dispatchMessages()
    {
        MachineMessage msg;
        while(m_from_impl->read(&msg, 1))
        {
            if(msg.opcode == PickDestination)
            {
                m_dst_iface = (Machine*) msg.value;
            }
            else
            {
                if(m_dst_iface == nullptr)
                {
                    
                }
                else
                {
                    m_dst_iface->dispatchMessage(msg);
                }
            }
        }
    }
    
    void startImplThread();
    
    void stopImplThread();
};


class MachineManagerImpl{
    friend class MachineImpl;
    
    CircularBuffer<MachineMessage>* m_to_impl;
    CircularBuffer<MachineMessage>* m_from_impl;
    
    MachineImpl* m_dst_impl  = nullptr;
    Machine*     m_dst_iface = nullptr;
    
    bool m_running = true;
    
public:
    MachineManagerImpl(CircularBuffer<MachineMessage>* to_impl, CircularBuffer<MachineMessage>* from_impl)
    : m_to_impl(to_impl)
    , m_from_impl(from_impl)
    {
        
    }
    
    virtual ~MachineManagerImpl()
    {
        
    }
    
    void sendMessages(const MachineMessage* msgs, int nmsgs)
    {
        m_from_impl->write(msgs, nmsgs);
    }

    void sendMessages(Machine* dst, const MachineMessage* msgs, int nmsgs)
    {
        if(dst != m_dst_iface)
            pickDestination(dst);
        sendMessages(msgs, nmsgs);
    }
    
    void pickDestination(Machine* dst)
    {
        MachineMessage msg(PickDestination, (unsigned long)dst);
        sendMessages(&msg, 1);
        m_dst_iface = dst;
    }

    void dispatchMessages()
    {
        MachineMessage msg;
        while(m_to_impl->read(&msg, 1))
        {
            if(msg.opcode == PickDestination)
            {
                m_dst_impl = (MachineImpl*) msg.value;
            }
            else
            {
                if(m_dst_impl == nullptr)
                {
                    if(msg.opcode == Terminate)
                    {
                        m_running = false;
                    }
                }
                else
                {
                    m_dst_impl->dispatchMessage(msg);
                }
            }
        }
    }
    
    void run()
    {
        while(m_running)
        {
            dispatchMessages();
        }
    }
};
    

void MachineManagerPrivate::startImplThread()
{
    struct Args{
        CircularBuffer<MachineMessage>* to_impl;
        CircularBuffer<MachineMessage>* from_impl;
    };
    
    auto args = new Args;
    args->to_impl = m_to_impl;
    args->from_impl = m_from_impl;
    
    m_thread->run([](void* arg) -> void* {
        auto args = (Args*) arg;
        auto impl = new MachineManagerImpl(args->to_impl, args->from_impl);
        delete args;
        impl->run();
        delete impl;
        return nullptr;
    }, args);
}


void MachineManagerPrivate::stopImplThread()
{
    MachineMessage msg(Terminate, 0);
    sendMessages(nullptr, &msg, 1);
    m_thread->join();
}


Machine::Machine(MachineManager* manager)
: m_manager_private(manager->m)
{
    
}


Machine::~Machine()
{
    
}


void Machine::sendMessage(const MachineMessage &msg)
{
    sendMessages(&msg, 1);
}
    
    
void Machine::sendMessages(const MachineMessage* msgs, int nmsgs)
{
    m_manager_private->sendMessages(this->impl(), msgs, nmsgs);
}
    
    

MachineImpl::MachineImpl(MachineManagerImpl* manager_impl, Machine* iface)
: m_manager_impl(manager_impl)
, m_iface(iface)
{
    
}
  
  
MachineImpl::~MachineImpl()
{
    
}


void MachineImpl::sendMessage(const MachineMessage &msg)
{
    sendMessages(&msg, 1);
}
    
    
void MachineImpl::sendMessages(const MachineMessage* msgs, int nmsgs)
{
    m_manager_impl->sendMessages(this->iface(), msgs, nmsgs);
}

}//namespace r64fx