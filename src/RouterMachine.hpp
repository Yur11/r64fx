#ifndef R64FX_ROUTER_MACHINE_HPP
#define R64FX_ROUTER_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{
    
class MachineConnection : public LinkedList<MachineConnection>::Node{
    MachineSignalSource* m_src_port  = nullptr;
    MachineSignalSink*   m_dst_port  = nullptr;
    
public:
    MachineConnection(MachineSignalSource* src_port, MachineSignalSink* dst_port);
};
  

class RouterMachine : public Machine{
    LinkedList<MachineConnection> m_connections;
    
public:
    RouterMachine(MachinePool* pool);
    
    virtual ~RouterMachine();
    
    MachineConnection* makeConnection(MachineSignalSource* src_port, MachineSignalSink* dst_port);
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
protected:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_ROUTER_MACHINE_HPP