#ifndef R64FX_MACHINE_MANAGER_HPP
#define R64FX_MACHINE_MANAGER_HPP

#include "Machine.hpp"

namespace r64fx{

class MachineManagerPrivate;
    
class MachineManager{
    friend class Machine;
    
    MachineManagerPrivate* m = nullptr;
    
public:
    MachineManager();
    
    virtual ~MachineManager();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_MANAGER_HPP