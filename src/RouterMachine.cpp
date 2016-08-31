#include "RouterMachine.hpp"
#include "MachineImpl.hpp"
#include "MachinePortImpl.hpp"
#include "MachineConnectionImpl.hpp"
#include "MachinePoolContext.hpp"
#include "SignalGraph.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{
    RouterMachine* g_router_machine_singleton_instance = nullptr;

    enum{
        MakeConnection = 1,
        BreakConnection,
        UpdateConnection,

        ConnectionMade,
        ConnectionFailed,
        ConnectionBroken,
        ConnectionUpdated,
        ConnectionUpdateFailed
    };
}//namespace

struct MachineConnectionSpec{
    MachineConnection* connection = nullptr; //Never use in impl thread!

    MachineConnectionImpl* connection_impl = nullptr;
    MachineSourceImpl*  source_impl  = nullptr;
    MachineSinkImpl*    sink_impl    = nullptr;
    MachineConnection::Mapping mapping = MachineConnection::Mapping::Default;
};


class RouterMachineImpl : public MachineImpl{
    virtual void deploy()
    {

    }

    virtual void withdraw()
    {

    }

    virtual void dispatchMessage(const MachineMessage &msg)
    {
        if(msg.opcode == MakeConnection)
        {
            auto spec = (MachineConnectionSpec*)msg.value;
            makeConnection(spec);
            sendMessage(spec->connection_impl ? ConnectionMade : ConnectionFailed, (unsigned long)spec);
        }
        else if(msg.opcode == BreakConnection)
        {
            auto spec = (MachineConnectionSpec*)msg.value;
            breakConnection(spec);
            sendMessage(ConnectionBroken, (unsigned long)spec);
        }
        else if(msg.opcode == UpdateConnection)
        {
            auto spec = (MachineConnectionSpec*)msg.value;
            updateConnection(spec);
            sendMessage(spec->connection_impl ? ConnectionUpdated : ConnectionUpdateFailed, (unsigned long)spec);
        }
    }

    void makeConnection(MachineConnectionSpec* spec)
    {
        MachineConnectionImpl* connection_impl = nullptr;
        auto source_impl  = spec->source_impl;
        auto sink_impl    = spec->sink_impl;
        auto mapping      = spec->mapping;

        if(mapping == MachineConnection::Mapping::Default)
        {
            if(source_impl->sources.size() == sink_impl->sinks.size())
            {
                connection_impl = new MachineConnectionImpl(source_impl->sources.size());
                for(unsigned long i=0; i<spec->source_impl->sources.size(); i++)
                {
                    auto connection = new SignalConnection(source_impl->sources.at(i), sink_impl->sinks.at(i));
                    ctx()->main_subgraph->addItem(connection);
                    connection_impl->at(i) = connection;
                }
            }
            else if(source_impl->sources.size() == 1)
            {
                connection_impl = new MachineConnectionImpl(sink_impl->sinks.size());
                for(unsigned long i=0; i<sink_impl->sinks.size(); i++)
                {
                    auto connection = new SignalConnection(source_impl->sources.at(0), sink_impl->sinks.at(i));
                    ctx()->main_subgraph->addItem(connection);
                    connection_impl->at(i)= connection;
                }
            }
            else
            {
                cerr << "Unsupported default mapping configuration: "
                     << spec->source_impl->sources.size() << " -> " << spec->sink_impl->sinks.size() << "\n";
            }
        }
        else
        {
            cerr << "Non default mappings not implemented!\n";
        }

        if(connection_impl)
        {
            spec->connection_impl = connection_impl;
        }
    }

    void breakConnection(MachineConnectionSpec* spec)
    {
        auto impl = spec->connection_impl;

        for(unsigned long i=0; i<impl->size(); i++)
        {
            auto connection = impl->at(i);
            ctx()->main_subgraph->removeItem(connection);
            delete connection;
        }

        delete impl;
    }

    void updateConnection(MachineConnectionSpec* spec)
    {
        breakConnection(spec);
        spec->connection_impl = nullptr;
        makeConnection(spec);
    }

    virtual void cycleStarted()
    {

    }

    virtual void cycleEnded()
    {

    }
};


RouterMachine::RouterMachine(MachinePool* pool)
: Machine(pool)
{
    setImpl(new RouterMachineImpl);
}


RouterMachine::~RouterMachine()
{

}


MachineConnection* RouterMachine::makeConnection(
    MachineSignalSource*  source_port,
    MachineSignalSink*    sink_port,
    MachineConnection::Mapping mapping
)
{
    if(source_port->impl() == nullptr || sink_port->impl() == nullptr)
    {
        cerr << "makeConnection: Some handles are null! " << source_port->impl() << ", " << sink_port->impl() << "\n";
        return nullptr;
    }

    MachineConnection* connection = find(source_port, sink_port);
    if(!connection)
    {
        connection = new MachineConnection(source_port, sink_port, mapping);

        auto spec = new MachineConnectionSpec;
        spec->connection = connection;
        spec->source_impl = source_port->impl();
        spec->sink_impl = sink_port->impl();

        sendMessage(MakeConnection, (unsigned long)spec);
    }
    return connection;
}


void RouterMachine::breakConnection(MachineConnection* connection)
{
    auto spec = new MachineConnectionSpec;
    spec->connection = connection;
    spec->connection_impl = connection->impl();

    sendMessage(BreakConnection, (unsigned long)spec);
}


void RouterMachine::updateConnection(MachineConnection* connection, bool pack)
{
    auto spec = new MachineConnectionSpec;
    spec->connection       = connection;
    spec->connection_impl  = connection->impl();
    spec->source_impl      = connection->sourcePort()->impl();
    spec->sink_impl        = connection->sinkPort()->impl();

    MachineMessage msg(UpdateConnection, (unsigned long)spec);
    if(pack)
    {
        packMessage(msg);
    }
    else
    {
        sendMessage(msg);
    }
}


void RouterMachine::packConnectionUpdatesFor(MachineSignalSource* source)
{
    auto recs = bySource(source);
    for(auto rec : *recs)
    {
        updateConnection(rec->connection(), true);
    }
}


void RouterMachine::packConnectionUpdatesFor(MachineSignalSink* sink)
{
    auto recs = bySink(sink);
    cout << "recs: " << recs << " -> " << sink << "\n";
    for(auto rec : *recs)
    {
        updateConnection(rec->connection(), true);
    }
}


SignalSourceConnectionRecord* RouterMachine::bySource(MachineSignalSource* source_port)
{
    return source_port->connection_record;
}


SignalSinkConnectionRecord* RouterMachine::bySink(MachineSignalSink* sink_port)
{
    return sink_port->connection_record;
}


MachineConnection* RouterMachine::find(MachineSignalSource* source, MachineSignalSink* sink)
{
    auto source_record = bySource(source);
    if(source_record)
    {
        return source_record->findSink(sink);
    }
    return nullptr;
}



RouterMachine* RouterMachine::singletonInstance(MachinePool* pool)
{
    if(!g_router_machine_singleton_instance)
    {
        g_router_machine_singleton_instance = new RouterMachine(pool);
        g_router_machine_singleton_instance->deploy();
    }
    return g_router_machine_singleton_instance;
}


void RouterMachine::cleanup()
{
    if(g_router_machine_singleton_instance)
    {
        g_router_machine_singleton_instance->withdraw();
        delete g_router_machine_singleton_instance;
    }
}


void RouterMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{

}


void RouterMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == ConnectionMade)
    {
        connectionMade((MachineConnectionSpec*)msg.value);
    }
    else if(msg.opcode == ConnectionFailed)
    {
        connectionFailed((MachineConnectionSpec*)msg.value);
    }
    else if(msg.opcode == ConnectionBroken)
    {
        connectionBroken((MachineConnectionSpec*)msg.value);
    }
    else if(msg.opcode == ConnectionUpdated)
    {
        connectionUpdated((MachineConnectionSpec*)msg.value);
    }
    else if(msg.opcode == ConnectionUpdateFailed)
    {
        connectionUpdateFailed((MachineConnectionSpec*)msg.value);
    }
}


void RouterMachine::connectionMade(MachineConnectionSpec* spec)
{
    spec->connection->setImpl(spec->connection_impl);
    storeConnection(spec->connection);
    delete spec;
}


void RouterMachine::connectionFailed(MachineConnectionSpec* spec)
{
    delete spec;
}


void RouterMachine::connectionBroken(MachineConnectionSpec* spec)
{
    removeConnection(spec->connection);
    delete spec->connection;
    delete spec;
}


void RouterMachine::connectionUpdated(MachineConnectionSpec* spec)
{
    spec->connection->setImpl(spec->connection_impl);
    delete spec;
}


void RouterMachine::connectionUpdateFailed(MachineConnectionSpec* spec)
{
    delete spec;
}


void RouterMachine::storeConnection(MachineConnection* connection)
{
    auto source_port = connection->sourcePort();
    auto sink_port = connection->sinkPort();

    auto source_record = bySource(source_port);
    auto sink_record = bySink(sink_port);

    if(!source_record)
    {
        source_record = new SignalSourceConnectionRecord(source_port);
        source_port->connection_record = source_record;
    }

    if(!sink_record)
    {
        sink_record = new SignalSinkConnectionRecord(sink_port);
        sink_port->connection_record = sink_record;
    }

    source_record->add(new MachineConnectionRecord(connection));
    sink_record->add(new MachineConnectionRecord(connection));
}


void RouterMachine::removeConnection(MachineConnection* connection)
{
    auto source_port = connection->sourcePort();
    auto sink_port = connection->sinkPort();

    auto source_record = bySource(source_port);
    auto sink_record = bySink(sink_port);

    if(source_record)
    {
        auto rec = source_record->find(connection);
        if(rec)
        {
            source_record->remove(rec);
        }

        if(source_record->isEmpty())
        {
            source_port->connection_record = nullptr;
            delete source_record;
        }
    }

    if(sink_record)
    {
        auto rec = sink_record->find(connection);
        if(rec)
        {
            sink_record->remove(rec);
        }

        if(sink_record->isEmpty())
        {
            sink_port->connection_record = nullptr;
            delete sink_record;
        }
    }
}


MachineConnectionRecord* PortConnectionRecord::find(MachineConnection* connection)
{
    for(auto rec : m_records)
    {
        if(rec->connection() == connection)
        {
            return rec;
        }
    }
    return nullptr;
}


MachineConnection* SignalSourceConnectionRecord::findSink(MachineSignalSink* sink) const
{
    for(auto record : *this)
    {
        if(record->connection()->sinkPort() == sink)
        {
            return record->connection();
        }
    }
    return nullptr;
}


MachineConnection* SignalSinkConnectionRecord::findSource(MachineSignalSource* source) const
{
    for(auto record : *this)
    {
        if(record->connection()->sourcePort() == source)
        {
            return record->connection();
        }
    }
    return nullptr;
}

}//namespace r64fx