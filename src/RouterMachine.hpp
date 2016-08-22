#ifndef R64FX_ROUTER_MACHINE_HPP
#define R64FX_ROUTER_MACHINE_HPP

#include "Machine.hpp"
#include "MachineConnection.hpp"
#include "MachineConnectionDatabase.hpp"

namespace r64fx{
    
class MachineConnectionSpec;
    
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
};
    
}//namespace r64fx

#endif//R64FX_ROUTER_MACHINE_HPP