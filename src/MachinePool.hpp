#ifndef R64FX_MACHINE_POOL_HPP
#define R64FX_MACHINE_POOL_HPP

#include "Machine.hpp"

namespace r64fx{

class MachinePoolPrivate;
    
class MachinePool{
    friend class Machine;
    
    MachinePoolPrivate* m = nullptr;
    
public:
    MachinePool();
    
    virtual ~MachinePool();
    
    LinkedList<Machine>::Iterator begin() const;
    
    LinkedList<Machine>::Iterator end() const;
    
    void withdrawAll();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_HPP