#ifndef R64FX_MACHINE_POOL_HPP
#define R64FX_MACHINE_POOL_HPP

#include "Machine.hpp"
#include "MachinePoolThread.hpp"

namespace r64fx{

class MachinePoolPrivate;
    
class MachinePool{
    friend class Machine;
    
    LinkedList<MachinePoolThread> m_threads;
    LinkedList<Machine>           m_machines;
    
public:
    MachinePool();
    
    virtual ~MachinePool();
    
    LinkedList<Machine>::Iterator begin() const;
    
    LinkedList<Machine>::Iterator end() const;
    
    MachinePoolThread* getThread() const;
    
private:
    MachinePoolThread* startNewThread();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_HPP
