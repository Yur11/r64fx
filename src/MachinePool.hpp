#ifndef R64FX_MACHINE_POOL_HPP
#define R64FX_MACHINE_POOL_HPP

#include "IteratorPair.hpp"
#include "MachineIface.hpp"
#include "MachinePoolThread.hpp"

namespace r64fx{

class MachinePool{
    friend class MachineIface;

    LinkedList<MachinePoolThread> m_threads;
    LinkedList<MachineIface>      m_machines;

public:
    MachinePool();

    virtual ~MachinePool();

    void deployMachine(MachineIface* machine);
    
    void withdrawMachine(MachineIface* machine);
    
    void withdrawAllMachines();

    IteratorPair<LinkedList<MachineIface>::Iterator> machines() const;
    
    MachinePoolThread* getThread() const;

private:
    MachinePoolThread* startNewThread();
};

}//namespace r64fx

#endif//R64FX_MACHINE_POOL_HPP
