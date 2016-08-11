#include "Machine.hpp"
#include "MachineImpl.hpp"
#include "MachinePool.hpp"
#include "Thread.hpp"
#include "Timer.hpp"
#include "CircularBuffer.hpp"
#include "MachinePoolContext.hpp"
#include "sleep.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    constexpr unsigned long PickDestination = 0;
    
    constexpr unsigned long Terminate       = 1;
    constexpr unsigned long Deploy          = 2;
    constexpr unsigned long Withdraw        = 3;
    constexpr unsigned long WithdrawAll     = 4;
}//namespace
    
    
class MachinePoolPrivate{
    MachinePool* m_pool = nullptr;
    
    Thread*  m_thread = nullptr;
    Timer*   m_timer  = nullptr;
    
    CircularBuffer<MachineMessage>* m_to_impl   = nullptr;
    CircularBuffer<MachineMessage>* m_from_impl = nullptr;
  
    MachineImpl* m_dst_impl  = nullptr;
    Machine*     m_dst_iface = nullptr;
    
    LinkedList<Machine> m_machines;
    
public:
    MachinePoolPrivate(MachinePool* pool)
    {
        m_pool = pool;
        m_thread     = new Thread;
        m_timer      = new Timer;
        m_to_impl    = new CircularBuffer<MachineMessage>(32);
        m_from_impl  = new CircularBuffer<MachineMessage>(32);
        
        startImplThread();
        
        m_timer->onTimeout([](Timer* timer, void* arg){
            auto self = (MachinePoolPrivate*) arg;
            self->dispatchMessages();
        }, this);
        m_timer->setInterval(500);
        m_timer->start();
    }
    
    ~MachinePoolPrivate()
    {
        m_timer->stop();
        
        stopImplThread();
        
        delete m_thread;
        delete m_timer;
        delete m_to_impl;
        delete m_from_impl;
    }
    
    void addMachine(Machine* machine)
    {
        m_machines.append(machine);
    }
    
    void removeMachine(Machine* machine)
    {
        m_machines.remove(machine);
    }
    
    MachinePool* pool() const
    {
        return m_pool;
    }
    
    LinkedList<Machine>::Iterator begin() const
    {
        return m_machines.begin();
    }
    
    LinkedList<Machine>::Iterator end() const
    {
        return m_machines.end();
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


class MachineThread{
    friend class MachineImpl;
    
    CircularBuffer<MachineMessage>* m_to_impl;
    CircularBuffer<MachineMessage>* m_from_impl;
    
    MachineImpl* m_dst_impl  = nullptr;
    Machine*     m_dst_iface = nullptr;
    
    bool m_running = true;
    
    MachinePoolContext* m_ctx = nullptr;
    
    LinkedList<MachineImpl> m_machines;
    
public:
    MachineThread(CircularBuffer<MachineMessage>* to_impl, CircularBuffer<MachineMessage>* from_impl)
    : m_to_impl(to_impl)
    , m_from_impl(from_impl)
    {
        m_ctx = new MachinePoolContext;
    }
    
    virtual ~MachineThread()
    {
        delete m_ctx;
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
                    else if(msg.opcode == Deploy)
                    {
                        auto machine_impl = (MachineImpl*) msg.value;
                        deployMachine(machine_impl);
                    }
                    else if(msg.opcode == Withdraw)
                    {
                        auto machine_impl = (MachineImpl*) msg.value;
                        withdrawMachine(machine_impl);
                    }
                    else if(msg.opcode == WithdrawAll)
                    {
                        withdrawAll();
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
            m_ctx->process();
            sleep_microseconds(500);
        }
    }
    
    void deployMachine(MachineImpl* machine_impl)
    {
        if(machine_impl)
        {
            if(machine_impl->thread() == nullptr)
            {
                m_machines.append(machine_impl);
                machine_impl->setThread(this);
                machine_impl->setContext(m_ctx);
                machine_impl->deploy();
            }
        }
    }
    
    void withdrawMachine(MachineImpl* machine_impl)
    {
        if(machine_impl)
        {
            if(machine_impl->thread() == this)
            {
                machine_impl->withdraw();
                machine_impl->setThread(nullptr);
                machine_impl->setContext(nullptr);
                m_machines.remove(machine_impl);
            }
        }
    }
    
    void withdrawAll()
    {
        auto machine_impl = m_machines.first();
        while(machine_impl)
        {
            withdrawMachine(machine_impl);
            machine_impl = m_machines.first();
        }
    }
};
    

void MachinePoolPrivate::startImplThread()
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
        auto mt = new MachineThread(args->to_impl, args->from_impl);
        delete args;
        mt->run();
        delete mt;
        return nullptr;
    }, args);
}


void MachinePoolPrivate::stopImplThread()
{
    MachineMessage msg(Terminate, 0);
    sendMessages(nullptr, &msg, 1);
    m_thread->join();
}


Machine::Machine(MachinePool* pool)
: m_pool_private(pool->m)
{
    m_pool_private->addMachine(this);
}


Machine::~Machine()
{
    m_pool_private->removeMachine(this);
}


MachinePool* Machine::pool() const
{
    return m_pool_private->pool();
}


void Machine::deploy()
{
    if(!m_is_deployed)
    {
        MachineMessage msg(Deploy, (unsigned long)impl());
        m_pool_private->sendMessages(nullptr, &msg, 1);
        m_is_deployed = true;
    }
}
    
    
void Machine::withdraw()
{
    if(m_is_deployed)
    {
        MachineMessage msg(Withdraw, (unsigned long)impl());
        m_pool_private->sendMessages(nullptr, &msg, 1);
        m_is_deployed = false;
    }
}


void Machine::setImpl(MachineImpl* impl)
{
    m_impl = impl;
    m_impl->setIface(this);
}
    
    
MachineImpl* Machine::impl() const
{
    return m_impl;
}


void Machine::sendMessage(unsigned long opcode, unsigned long value)
{
    sendMessage(MachineMessage(opcode, value));
}


void Machine::sendMessage(const MachineMessage &msg)
{
    sendMessages(&msg, 1);
}
    
    
void Machine::sendMessages(const MachineMessage* msgs, int nmsgs)
{
    m_pool_private->sendMessages(this->impl(), msgs, nmsgs);
}
    
      
void MachineImpl::sendMessage(unsigned long opcode, unsigned long value)
{
    sendMessage(MachineMessage(opcode, value));
}


void MachineImpl::sendMessage(const MachineMessage &msg)
{
    sendMessages(&msg, 1);
}
    
    
void MachineImpl::sendMessages(const MachineMessage* msgs, int nmsgs)
{
    m_pool_impl->sendMessages(this->iface(), msgs, nmsgs);
}


MachinePool::MachinePool()
{
    m = new MachinePoolPrivate(this);
}
    

MachinePool::~MachinePool()
{
    delete m;
}


LinkedList<Machine>::Iterator MachinePool::begin() const
{
    return m->begin();
}
    
    
LinkedList<Machine>::Iterator MachinePool::end() const
{
    return m->end();
}


void MachinePool::withdrawAll()
{
    MachineMessage msg(WithdrawAll, 0);
    m->sendMessages(nullptr, &msg, 1);
}

}//namespace r64fx