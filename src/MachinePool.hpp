#ifndef R64FX_MACHINE_POOL_HPP
#define R64FX_MACHINE_POOL_HPP

#include "MachineIface.hpp"
#include "MachinePoolThread.hpp"

namespace r64fx{

class MachinePoolPrivate;
    
class MachinePool{
    friend class MachineIface;
    
    LinkedList<MachinePoolThread> m_threads;
    LinkedList<MachineIface>      m_machines;
    
public:
    MachinePool();
    
    virtual ~MachinePool();
    
    LinkedList<MachineIface>::Iterator begin() const;
    
    LinkedList<MachineIface>::Iterator end() const;
    
    MachinePoolThread* getThread() const;
    
private:
    MachinePoolThread* startNewThread();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_HPP
