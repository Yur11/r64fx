#ifndef R64FX_ROUTER_MACHINE_HPP
#define R64FX_ROUTER_MACHINE_HPP

#include "Machine.hpp"
#include "MachineConnection.hpp"

namespace r64fx{
    
class MachineConnectionSpec;
class SignalSourceConnectionRecord;
class SignalSinkConnectionRecord;
    
class RouterMachine : public Machine{
    RouterMachine(MachinePool* pool);
    
    virtual ~RouterMachine();
    
public:
    MachineConnection* makeConnection(
        MachineSignalSource*  src_port, 
        MachineSignalSink*    dst_port, 
        MachineConnection::Mapping mapping = MachineConnection::Mapping::Default
    );
    
    void breakConnection(MachineConnection* connection);
    
    void updateConnection(MachineConnection* connection, bool pack = false);
    
    void packConnectionUpdatesFor(MachineSignalSource* source);
    
    void packConnectionUpdatesFor(MachineSignalSink* sink);
    
    SignalSourceConnectionRecord* bySource(MachineSignalSource* source);
    
    SignalSinkConnectionRecord* bySink(MachineSignalSink* sink);
    
    MachineConnection* find(MachineSignalSource* source, MachineSignalSink* sink);
    
    static RouterMachine* singletonInstance(MachinePool* pool);
    
    static void cleanup();
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
private:
    virtual void dispatchMessage(const MachineMessage &msg);
    
    void connectionMade(MachineConnectionSpec* spec);
    
    void connectionFailed(MachineConnectionSpec* spec);
    
    void connectionBroken(MachineConnectionSpec* spec);
    
    void connectionUpdated(MachineConnectionSpec* spec);
    
    void connectionUpdateFailed(MachineConnectionSpec* spec);
    
    void storeConnection(MachineConnection* connection);
    
    void removeConnection(MachineConnection* connection);
};


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
    
}//namespace r64fx

#endif//R64FX_ROUTER_MACHINE_HPP