#include "MachinePort.hpp"

namespace r64fx{
    
MachinePort::MachinePort(Machine* machine, const std::string &name)
: m_machine(machine)
, m_name(name)
{
    
}
    
    
Machine* MachinePort::machine() const
{
    return m_machine;
}


void MachinePort::setHandle(void* handle)
{
    m_handle = handle;
}


void* MachinePort::handle() const
{
    return m_handle;
}


void MachinePort::setName(const std::string &name)
{
    m_name = name;
}


std::string MachinePort::name() const
{
    return m_name;
}
 
}///namespace r64fx