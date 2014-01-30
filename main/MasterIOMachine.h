#ifndef R64FX_MAIN_MASTER_IO_MACHINE_H
#define R64FX_MAIN_MASTER_IO_MACHINE_H

#include "Machine.h"

namespace r64fx{
  
class MasterIOMachine : public Machine{
    std::vector<float*> master_input_buffers;
    std::vector<float*> master_output_buffers;
    
public:
    MasterIOMachine(FrontMachineScene* fms, BackMachineScene* bms);
    
    
};
    
}//namespace r64fx

#endif//R64FX_MAIN_MASTER_IO_MACHIENE_H