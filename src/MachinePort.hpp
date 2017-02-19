#ifndef R64FX_R64FX_MACHINE_PORT_HPP
#define R64FX_R64FX_MACHINE_PORT_HPP

#include <string>
#include "LinkedList.hpp"

namespace r64fx{

class MachineIface;
class MachineSourceImpl;
class MachineSinkImpl;
class SignalSourceConnectionRecord;
class SignalSinkConnectionRecord;

class MachinePort : public LinkedList<MachinePort>::Node{
    MachineIface* m_machine = nullptr;
    std::string m_name = "";
    int m_component_count = 0;

protected:
    unsigned int m_flags = 0;

public:
    MachinePort(MachineIface* machine, const std::string &name, int component_count = 1);

    MachineIface* machine() const;

    void setName(const std::string &name);

    std::string name() const;

    int componentCount() const;

    bool isSignalPort() const;

    bool isSequencerPort() const;

    bool isSink() const;

    bool isSource() const;
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
    friend class MachineConnectionDatabase;
    friend class RouterMachine;

    SignalSinkConnectionRecord* connection_record = nullptr;
    MachineSinkImpl* m_impl = nullptr;

public:
    using MachineSignalPort::MachineSignalPort;

    inline void setImpl(MachineSinkImpl* impl)
    {
        m_impl = impl;
    }

    inline MachineSinkImpl* impl() const
    {
        return m_impl;
    }
};


class MachineSignalSource : public MachineSignalPort{
    friend class MachineConnectionDatabase;
    friend class RouterMachine;

    SignalSourceConnectionRecord* connection_record = nullptr;
    MachineSourceImpl* m_impl = nullptr;

public:
    using MachineSignalPort::MachineSignalPort;

    inline void setImpl(MachineSourceImpl* impl)
    {
        m_impl = impl;
    }

    inline MachineSourceImpl* impl() const
    {
        return m_impl;
    }
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
