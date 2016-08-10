#ifndef R64FX_R64FX_MACHINE_PORT_HPP
#define R64FX_R64FX_MACHINE_PORT_HPP

#include <string>
#include "LinkedList.hpp"

namespace r64fx{
    
class Machine;
    
class MachinePort : public LinkedList<MachinePort>::Node{
    Machine* m_machine = nullptr;
    void* m_handle = 0;
    std::string m_name = "";
    
public:
    MachinePort(Machine* machine, const std::string &name);
    
    Machine* machine() const;
    
    void setHandle(void* handle);
    
    void* handle() const;
    
    void setName(const std::string &name);
    
    std::string name() const;
};


class MachineSignalPort : public MachinePort{
public:
    using MachinePort::MachinePort;
};


class MachineSequencerPort : public MachinePort{
public:
    using MachinePort::MachinePort;    
};


class MachineSignalSink : public MachineSignalPort{
public:
    using MachineSignalPort::MachineSignalPort;
};


class MachineSignalSource : public MachineSignalPort{
public:
    using MachineSignalPort::MachineSignalPort;    
};


class MachineSequencerSink : public MachineSequencerPort{
public:
    using MachineSequencerPort::MachineSequencerPort;
};


class MachineSequencerSource : public MachineSequencerPort{
public:
    using MachineSequencerPort::MachineSequencerPort;    
};

    
}//namespace r64fx

#endif//R64FX_R64FX_MACHINE_PORT_HPP