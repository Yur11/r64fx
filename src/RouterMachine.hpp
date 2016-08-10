#ifndef R64FX_ROUTER_MACHINE_HPP
#define R64FX_ROUTER_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{
    
class RouterMachineRule : public LinkedList<RouterMachineRule>::Node{
    MachineSignalSource* m_src_port  = nullptr;
    MachineSignalSink*   m_dst_port  = nullptr;
    
public:
    RouterMachineRule(MachineSignalSource* src_port, MachineSignalSink* dst_port);
};
  

class RouterMachine : public Machine{
    LinkedList<RouterMachineRule> m_rules;
    
public:
    RouterMachine(MachinePool* pool);
    
    virtual ~RouterMachine();
    
    RouterMachineRule* createRule(MachineSignalSource* src_port, MachineSignalSink* dst_port);
    
protected:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_ROUTER_MACHINE_HPP