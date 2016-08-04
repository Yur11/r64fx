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
}//namespace
    
    
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
    }
    
    ~MachineManagerPrivate()
    {
        delete m_thread;
        delete m_timer;
        delete m_to_impl;
        delete m_from_impl;
    }
    
    void sendMessages(MachineMessage* msgs, int nmsgs)
    {
        m_to_impl->write(msgs, nmsgs);
    }

    void sendMessages(MachineImpl* dst, MachineMessage* msgs, int nmsgs)
    {
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
};


class MachineManagerImpl{
    friend class MachineImpl;
    
    CircularBuffer<MachineMessage>* m_to_impl;
    CircularBuffer<MachineMessage>* m_from_impl;
    
    MachineImpl* m_dst_impl  = nullptr;
    Machine*     m_dst_iface = nullptr;
    
    bool m_running = true;
    
public:
    MachineManagerImpl(CircularBuffer<MachineMessage>* to_impl, CircularBuffer<MachineMessage>* from_impl);
    
    virtual ~MachineManagerImpl();
    
    void sendMessages(MachineMessage* msgs, int nmsgs)
    {
        m_from_impl->write(msgs, nmsgs);
    }

    void sendMessages(Machine* dst, MachineMessage* msgs, int nmsgs)
    {
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
};
    

    
}//namespace r64fx