#include "MonophonicMixer.h"

namespace r64fx{
    
MonophonicMixer::MonophonicMixer()
{
    Processor::block_size = 4;
    Processor::block_count = 1;
    
    _input_port.addr = new float[Processor::size()];
    _level_port.addr = new float[Processor::size()];
    
    _input_port.addr[0] = 
    _input_port.addr[1] = 
    _input_port.addr[2] = 
    _input_port.addr[3] = 0.0;
    
    _level_port.addr[0] = 
    _level_port.addr[1] = 
    _level_port.addr[2] = 
    _level_port.addr[3] = 1.0;
    
    _mix = new float; 
}
    

MonophonicMixer::~MonophonicMixer()
{
    delete[] _input_port.addr;
    delete[] _level_port.addr;
    delete _mix;
}
   

void MonophonicMixer::render(Assembler &as)
{
#ifdef DEBUG
    as.dump << "\n\n---MonophonicMixer---\n";
#endif//DEBUG
    as.movups(xmm0, Mem128(_input_port.addr));
    as.mulps(xmm0, Mem128(_level_port.addr));
    as.pshufd(xmm1, xmm0, shuf(1, 0, 0, 0));
    as.pshufd(xmm2, xmm0, shuf(2, 0, 0, 0));
    as.pshufd(xmm3, xmm0, shuf(3, 0, 0, 0));
    as.addss(xmm0, xmm1);
    as.addss(xmm2, xmm3);
    as.addss(xmm0, xmm2);
    as.movss(Mem32(_mix), xmm0);
}
    
}//namespace r64fx