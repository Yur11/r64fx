#include "SawtoothOsc.hpp"

namespace r64fx{

/* Remove this! */
float samplerate_rcp[4] = { 
    1.0/ 48000.0, 1.0/ 48000.0, 1.0/ 48000.0, 1.0/ 48000.0
};

float zero[4] = {
    0.0, 0.0, 0.0, 0.0
};

float half[4] = {
    0.5, 0.5, 0.5, 0.5
};

float one[4] = {
    1.0, 1.0, 1.0, 1.0
};

float negative_one[4] = {
    -1.0, -1.0, -1.0, -1.0
};

float two[4] = {
    2.0, 2.0, 2.0, 2.0
};
    

SawtoothOsc::SawtoothOsc()
{
    Processor::block_size = 4;
    Processor::block_count = 1;
    _frequency_port.addr = new float[Processor::size()];
    _fm_port.addr = new float[Processor::size()];
    _value_port.addr = new float[Processor::size()];
    _normalized_port.addr = new float[Processor::size()];
    
    myPort(&_frequency_port);
    myPort(&_fm_port);
    myPort(&_value_port);
    myPort(&_normalized_port);
    
    _frequency_port.addr[0] = 
    _frequency_port.addr[1] = 
    _frequency_port.addr[2] = 
    _frequency_port.addr[3] = 220.0;
    
    _fm_port.addr[0] = 
    _fm_port.addr[1] = 
    _fm_port.addr[2] = 
    _fm_port.addr[3] = 1.0;
    
    _value_port.addr[0] = 
    _value_port.addr[1] = 
    _value_port.addr[2] = 
    _value_port.addr[3] = 0.0;
    
    _normalized_port.addr[0] = 
    _normalized_port.addr[1] = 
    _normalized_port.addr[2] = 
    _normalized_port.addr[3] = 0.0;
}


SawtoothOsc::~SawtoothOsc()
{
    delete[] _frequency_port.addr;
    delete[] _fm_port.addr;
    delete[] _value_port.addr;
    delete[] _normalized_port.addr;
}


void SawtoothOsc::render(Assembler &as)
{
#ifdef DEBUG
    as.dump << "\n\n---SawtoothOsc---\n";
#endif//DEBUG
    as.movups(xmm0, Mem128(_frequency_port.addr));
    as.mulps(xmm0, Mem128(_fm_port.addr));
    as.mulps(xmm0, Mem128(samplerate_rcp));
    as.movups(xmm1, Mem128(_value_port.addr));
    as.addps(xmm1, xmm0);
    as.movaps(xmm2, Mem128(one));
    as.cmpltps(xmm2, xmm1);
    as.andps(xmm2, xmm1);
    as.subps(xmm1, xmm2);
    as.movups(Mem128(_value_port.addr), xmm1);

    as.subps(xmm1, Mem128(half));
    as.mulps(xmm1, Mem128(two));
    
    as.movups(Mem128(_normalized_port.addr), xmm1);
}
    
}//namespace r64fx