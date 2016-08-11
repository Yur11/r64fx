#include "MachineConnectionDatabase.hpp"

namespace r64fx{
    
    
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
    
    
void MachineConnectionDatabase::add(MachineConnection* connection)
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


void MachineConnectionDatabase::remove(MachineConnection* connection)
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


SignalSourceConnectionRecord* MachineConnectionDatabase::bySource(MachineSignalSource* source_port)
{
    return source_port->connection_record;
}
    
    
SignalSinkConnectionRecord* MachineConnectionDatabase::bySink(MachineSignalSink* sink_port)
{
    return sink_port->connection_record;
}


MachineConnection* MachineConnectionDatabase::find(MachineSignalSource* source, MachineSignalSink* sink)
{
    auto source_record = bySource(source);
    if(source_record)
    {
        return source_record->findSink(sink);
    }
    return nullptr;
}
    
}//namespace r64fx