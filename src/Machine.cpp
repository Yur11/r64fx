#include "Machine.hpp"
#include "MachineConnectionDatabase.hpp"
#include "MachineFlags.hpp"
#include "MachineImpl.hpp"
#include "MachinePool.hpp"
#include "MachinePoolContext.hpp"
#include "MachinePortImpl.hpp"
#include "MachineConnectionImpl.hpp"
#include "Thread.hpp"
#include "Timer.hpp"
#include "CircularBuffer.hpp"
#include "sleep.hpp"

#include <vector>

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    constexpr unsigned long PickDestination         = 0;
    
    constexpr unsigned long Terminate               = 1;
    constexpr unsigned long DispatchPack            = 2;
    constexpr unsigned long DeployMachine           = 3;
    constexpr unsigned long WithdrawMachine         = 4;
    constexpr unsigned long WithdrawAllMachines     = 5;
    constexpr unsigned long MakeConnection          = 6;

    constexpr unsigned long PackDispatched          = 7;
    constexpr unsigned long MachineDeployed         = 8;
    constexpr unsigned long MachineWithdrawn        = 9;
    constexpr unsigned long ConnectionResponse      = 10;

    
    
    struct MachineMessagePack : public LinkedList<MachineMessagePack>::Node{
        vector<MachineMessage> msgs;
        
        inline void append(const MachineMessage &msg)
        {
            msgs.push_back(msg);
        }
        
        inline void clear()
        {
            msgs.clear();
        }
        
        inline int size() const
        {
            return msgs.size();
        }
        
        inline bool empty() const
        {
            return msgs.empty();
        }
    };
    
   
    struct MachineConnectionSpec{
        /* In */
        MachineSourceImpl*  source_impl  = nullptr;
        MachineSinkImpl*    sink_impl    = nullptr;
        MachineConnection::Mapping mapping = MachineConnection::Mapping::Default;
        
        /* Out */
        MachineConnectionImpl* connection_impl = nullptr;
    };
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
    
    LinkedList<MachineMessagePack> m_spare_packs;
    MachineMessagePack* m_current_pack;
    MachineImpl* m_pack_dst = nullptr;
    
    
public:
    MachinePoolPrivate(MachinePool* pool)
    {
        m_pool = pool;
        m_thread     = new Thread;
        m_timer      = new Timer;
        m_to_impl    = new CircularBuffer<MachineMessage>(32);
        m_from_impl  = new CircularBuffer<MachineMessage>(32);
        
        m_current_pack = new MachineMessagePack;
        
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
    
    void packMessages(const MachineMessage* msgs, int nmsgs)
    {
        for(int i=0; i<nmsgs; i++)
        {
            m_current_pack->append(msgs[i]);
        }
    }

    void packMessages(MachineImpl* dst, const MachineMessage* msgs, int nmsgs)
    {
        if(dst != m_pack_dst)
            pickPackDestination(dst);
        packMessages(msgs, nmsgs);
    }
    
    void pickPackDestination(MachineImpl* dst)
    {
        MachineMessage msg(PickDestination, (unsigned long)dst);
        packMessages(&msg, 1);
        m_pack_dst = dst;
    }
    
    void sendPack()
    {
        if(!m_current_pack->empty())
        {
            m_current_pack->append(MachineMessage(PickDestination, 0));
            MachineMessage msg(DispatchPack, (unsigned long)m_current_pack);
            sendMessages(nullptr, &msg, 1);
            
            auto new_pack = m_spare_packs.last();
            if(new_pack)
            {
                m_spare_packs.remove(new_pack);
            }
            else
            {
                new_pack = new MachineMessagePack;
            }
            m_current_pack = new_pack;
        }
    }
    
    void clearPack()
    {
        if(!m_current_pack->empty())
        {
            m_current_pack->clear();
        }
    }
    
    int packSize()
    {
        return m_current_pack->size();
    }
    
    void packDispatched(MachineMessagePack* pack)
    {
        pack->clear();
        m_spare_packs.append(pack);
    }
    
    void dispatchMessages()
    {
        MachineMessage msg;
        while(m_from_impl->read(&msg, 1))
        {
            dispatchMessage(msg);
        }
    }
    
    void dispatchMessage(const MachineMessage &msg)
    {
        if(msg.opcode == PickDestination)
        {
            m_dst_iface = (Machine*) msg.value;
        }
        else
        {
            if(m_dst_iface == nullptr)
            {
                if(msg.opcode == DispatchPack)
                {

                }
                else if(msg.opcode == PackDispatched)
                {
                    packDispatched((MachineMessagePack*)msg.value);
                }
                else if(msg.opcode == MachineDeployed)
                {
                    m_dst_iface->m_flags &= ~R64FX_MACHINE_DEPLOYMENT_PENDING;
                    cout << "MachineDeployed\n";
                }
                else if(msg.opcode == MachineWithdrawn)
                {
                    m_dst_iface->m_flags &= ~R64FX_MACHINE_WITHDRAWAL_PENDING;
                    cout << "MachineWithdrawn\n";
                }
            }
            else
            {
                m_dst_iface->dispatchMessage(msg);
            }
        }
    }

    MachineConnection* makeConnection(
        MachineSignalSource* source_port, MachineSignalSink* sink_port, MachineConnection::Mapping mapping
    )
    {
        if(source_port->impl() == nullptr || sink_port->impl() == nullptr)
        {
            cerr << "makeConnection: Some handles are null! " << source_port->impl() << ", " << sink_port->impl() << "\n";
            return nullptr;
        }
        
        MachineConnection* connection = MachineConnectionDatabase::find(source_port, sink_port);
        if(!connection)
        {
            connection = new MachineConnection(source_port, sink_port, mapping);
            MachineConnectionDatabase::add(connection);

            auto spec = new MachineConnectionSpec;
            spec->source_impl = source_port->impl();
            spec->sink_impl = sink_port->impl();
            
            MachineMessage msg(MakeConnection, (unsigned long)spec);
            sendMessages(nullptr, &msg, 1);
        }
        return connection;
    }
    
    void breakConnection(MachineConnection* connection)
    {
        
    }
    
    void updateConnection(MachineConnection* connection)
    {
        
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
            dispatchMessage(msg);
        }
    }
    
    void dispatchMessage(const MachineMessage &msg)
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
                else if(msg.opcode == DispatchPack)
                {
                    dispatchPack((MachineMessagePack*)msg.value);
                }
                else if(msg.opcode == DeployMachine)
                {
                    auto machine_impl = (MachineImpl*) msg.value;
                    deployMachine(machine_impl);
                    
                }
                else if(msg.opcode == WithdrawMachine)
                {
                    auto machine_impl = (MachineImpl*) msg.value;
                    withdrawMachine(machine_impl);
                }
                else if(msg.opcode == WithdrawAllMachines)
                {
                    withdrawAll();
                }
                else if(msg.opcode == MakeConnection)
                {
                    cout << "opcode MakeConnection\n";
                    makeConnection((MachineConnectionSpec*)msg.value);
                }
            }
            else
            {
                m_dst_impl->dispatchMessage(msg);
            }
        }
    }
    
    void dispatchPack(MachineMessagePack* pack)
    {
        for(auto &msg : pack->msgs)
        {
            dispatchMessage(msg);
        }
        
        MachineMessage msg(PackDispatched, (unsigned long)pack);
        sendMessages(nullptr, &msg, 1);
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
                
                MachineMessage msg(MachineDeployed, 0);
                sendMessages(machine_impl->iface(), &msg, 1);
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
                
                MachineMessage msg(MachineWithdrawn, 0);
                sendMessages(machine_impl->iface(), &msg, 1);
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
    
    void makeConnection(MachineConnectionSpec* spec)
    {
        cout << "makeConnection(){}\n";
        
        if(spec->mapping == MachineConnection::Mapping::Default)
        {
            if(spec->source_impl->size() == spec->sink_impl->size())
            {
                spec->connection_impl = new MachineConnectionImpl(spec->source_impl->size());
                for(unsigned long i=0; i<spec->source_impl->size(); i++)
                {
                    auto connection = new SignalConnection(spec->source_impl->at(i), spec->sink_impl->at(i));
                    m_ctx->main_subgraph->addItem(connection);
                    spec->connection_impl->at(i) = connection;
                }
            }
            else if(spec->source_impl->size() == 1)
            {
                spec->connection_impl = new MachineConnectionImpl(spec->sink_impl->size());
                for(unsigned long i=0; i<spec->sink_impl->size(); i++)
                {
                    auto connection = new SignalConnection(spec->source_impl->at(0), spec->sink_impl->at(i));
                    m_ctx->main_subgraph->addItem(connection);
                    spec->connection_impl->at(i)= connection;
                }
            }
//             else if(spec->sink_impl->size() == 1)
//             {
//                 spec->connection_impl = new MachineConnectionImpl(spec->source_impl->size());
//                 for(unsigned long i=0; i<spec->source_impl->size(); i++)
//                 {
//                     auto connection = new SignalConnection(spec->source_impl->at(i), spec->sink_impl->at(0));
//                     m_ctx->main_subgraph->addItem(connection);
//                     spec->connection_impl->at(i)= connection;
//                 }
//             }
            else
            {
                cerr << "Unsupported default mapping configuration: " 
                     << spec->source_impl->size() << " -> " << spec->sink_impl->size() << "\n";
            }
        }
        else
        {
            cerr << "Non default mappings not implemented!\n";
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
        cout << "->>> Starting thread!\n";
        mt->run();
        cout << "<<<- Exiting thread!\n";
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
    MachineMessage msg(WithdrawAllMachines, 0);
    m->sendMessages(nullptr, &msg, 1);
}


MachineConnection* MachinePool::makeConnection(
    MachineSignalSource* source_port, MachineSignalSink* sink_port, MachineConnection::Mapping mapping
)
{
    return m->makeConnection(source_port, sink_port, mapping);
}


void MachinePool::breakConnection(MachineConnection* connection)
{
    m->breakConnection(connection);
}


void MachinePool::updateConnection(MachineConnection* connection)
{
    m->updateConnection(connection);
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
    m_thread->sendMessages(this->iface(), msgs, nmsgs);
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
    if(!isDeployed())
    {
        m_flags |= R64FX_MACHINE_DEPLOYMENT_PENDING;
        MachineMessage msg(DeployMachine, (unsigned long)impl());
        m_pool_private->sendMessages(nullptr, &msg, 1);
    }
}
    
    
void Machine::withdraw()
{
    if(isDeployed())
    {
        m_flags |= R64FX_MACHINE_WITHDRAWAL_PENDING;
        MachineMessage msg(WithdrawMachine, (unsigned long)impl());
        m_pool_private->sendMessages(nullptr, &msg, 1);
    }
}


bool Machine::isDeployed() const
{
    return m_flags & R64FX_MACHINE_DEPLOYED;
}


bool Machine::deploymentPending() const
{
    return m_flags & R64FX_MACHINE_DEPLOYMENT_PENDING;
}
    
    
bool Machine::withdrawalPending() const
{
    return m_flags & R64FX_MACHINE_WITHDRAWAL_PENDING;
}

    
void Machine::setName(const std::string &name)
{
    m_name = name;
}


std::string Machine::name() const
{
    return m_name;
}


bool Machine::isReady() const
{
    return m_flags & R64FX_MACHINE_IS_READY;
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


void Machine::packMessage(unsigned long opcode, unsigned long value)
{
    packMessage(MachineMessage(opcode, value));
}
    
    
void Machine::packMessage(const MachineMessage &msg)
{
    packMessages(&msg, 1);
}


void Machine::packMessages(const MachineMessage* msgs, int nmsgs)
{
    m_pool_private->packMessages(this->impl(), msgs, nmsgs);
}


void Machine::sendPack()
{
    m_pool_private->sendPack();
}


void Machine::clearPack()
{
    m_pool_private->clearPack();
}


int Machine::packSize() const
{
    return m_pool_private->packSize();
}


void Machine::packConnectionUpdatesFor(MachineSignalSource* source)
{
    cout << "packConnectionUpdatesForSource\n";
}
    
    
void Machine::packConnectionUpdatesFor(MachineSignalSink* sink)
{
    cout << "packConnectionUpdatesForSink\n";
}


void Machine::block()
{
    Timer::runTimers();
    sleep_microseconds(5000);
}

}//namespace r64fx