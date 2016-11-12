#ifndef R64FX_FILTER_MACHINE_HPP
#define R64FX_FILTER_MACHINE_HPP

#include "MachineIface.hpp"

namespace r64fx{

class FilterMachine : public MachineIface{
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

    virtual void forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg);
};

}//namespace r64fx

#endif//R64FX_FILTER_MACHINE_HPP
