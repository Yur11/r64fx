#ifndef R64FX_MACHINE_H
#define R64FX_MACHINE_H

#include "machine_scenes.h"

namespace r64fx{
    
class Machine{
    FrontMachineScene* _fms;
    BackMachineScene* _bms;
    
    MachineWidget* _front;
    MachineWidget* _back;
    
public:
    Machine(/*Graph* graph, */FrontMachineScene* fms, BackMachineScene* bms);
    
    virtual ~Machine();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_H