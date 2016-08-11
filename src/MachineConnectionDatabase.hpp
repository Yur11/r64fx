#ifndef R64FX_MACHINE_CONNECTION_DATABASE_HPP
#define R64FX_MACHINE_CONNECTION_DATABASE_HPP

#include "MachinePort.hpp"
#include "MachineConnection.hpp"

namespace r64fx{
    
/* A wrapper record for a connection. */    
class MachineConnectionRecord : public LinkedList<MachineConnectionRecord>::Node{
    MachineConnection* m_connection = nullptr;
    
public:
    MachineConnectionRecord(MachineConnection* mc)
    : m_connection(mc)
    {}
    
    inline MachineConnection* connection() const
    {
        return m_connection;
    }
};


class PortConnectionRecord{
    LinkedList<MachineConnectionRecord> m_records;
    
public:
    inline void add(MachineConnectionRecord* record)
    {
        m_records.append(record);
    }
    
    inline void remove(MachineConnectionRecord* record)
    {
        m_records.remove(record);
    }
    
    inline LinkedList<MachineConnectionRecord>::Iterator begin() const
    {
        return m_records.begin();
    }
    
    inline LinkedList<MachineConnectionRecord>::Iterator end() const
    {
        return m_records.end();
    }
    
    inline  bool isEmpty() const
    {
        return m_records.isEmpty();
    }
    
    MachineConnectionRecord* find(MachineConnection* connection);
};


class SignalSourceConnectionRecord 
: public LinkedList<SignalSourceConnectionRecord>::Node
, public PortConnectionRecord{
    MachineSignalSource* m_source = nullptr;
    
public:
    SignalSourceConnectionRecord(MachineSignalSource* source)
    : m_source(source)
    {
        
    }
    
    inline MachineSignalSource* source() const
    {
        return m_source;
    }
    
    MachineConnection* findSink(MachineSignalSink* sink) const;
};


class SignalSinkConnectionRecord
: public LinkedList<SignalSinkConnectionRecord>::Node
, public PortConnectionRecord{
    MachineSignalSink* m_sink = nullptr;
    
public:
    SignalSinkConnectionRecord(MachineSignalSink* sink)
    : m_sink(sink)
    {
        
    }
    
    inline MachineSignalSink* sink() const
    {
        return m_sink;
    }
    
    MachineConnection* findSource(MachineSignalSource* source) const;
};

    
class MachineConnectionDatabase{
public:
    static void add(MachineConnection* connection);
    
    static void remove(MachineConnection* connection);
    
    static SignalSourceConnectionRecord* bySource(MachineSignalSource* source);
    
    static SignalSinkConnectionRecord* bySink(MachineSignalSink* sink);
    
    static MachineConnection* find(MachineSignalSource* source, MachineSignalSink* sink);
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_CONNECTION_DATABASE_HPP