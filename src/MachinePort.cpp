#include "MachinePort.hpp"

#define R64FX_PORT_IS_SIGNAL       1
#define R64FX_PORT_IS_SEQUENCER    2
#define R64FX_PORT_IS_SINK         4

namespace r64fx{
    
MachinePort::MachinePort(MachineIface* machine, const std::string &name, int component_count)
: m_machine(machine)
, m_name(name)
, m_component_count(component_count)
{
    
}
    
    
MachineIface* MachinePort::machine() const
{
    return m_machine;
}


void MachinePort::setName(const std::string &name)
{
    m_name = name;
}


std::string MachinePort::name() const
{
    return m_name;
}

    
int MachinePort::componentCount() const
{
    return m_component_count;
}


bool MachinePort::isSignalPort() const
{
    return m_flags & R64FX_PORT_IS_SIGNAL;
}
    
    
bool MachinePort::isSequencerPort() const
{
    return m_flags & R64FX_PORT_IS_SEQUENCER;
}


bool MachinePort::isSink() const
{
    return m_flags & R64FX_PORT_IS_SINK;
}


bool MachinePort::isSource() const
{
    return !(m_flags & R64FX_PORT_IS_SINK);
}
 
}///namespace r64fx
