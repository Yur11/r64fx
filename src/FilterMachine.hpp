#ifndef R64FX_FILTER_MACHINE_HPP
#define R64FX_FILTER_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{

class FilterMachine : public Machine{
    MachineSignalSink    m_sink;
    MachineSignalSource  m_source;

    int m_size = 0;

public:
    FilterMachine(MachinePool* pool);

    virtual ~FilterMachine();

    void setSize(int size);

    int size() const;

    void setPole(float pole);

    MachineSignalSink* sink();

    MachineSignalSource* source();

    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);

public:
    virtual void dispatchMessage(const MachineMessage &msg);
};

}//namespace r64fx

#endif//R64FX_FILTER_MACHINE_HPP