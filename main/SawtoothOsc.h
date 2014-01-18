#ifndef R64FX_MAIN_SAWTOOTH_OSC_H
#define R64FX_MAIN_SAWTOOTH_OSC_H

#include "dsp/Processor.h"

namespace r64fx{
    
class SawtoothOsc : public Processor{
    InputPort _frequency_port;
    InputPort _fm_port;
    OutputPort _value_port;
    OutputPort _normalized_port;
    
public:
    SawtoothOsc();
    
    ~SawtoothOsc();
    
    virtual void render(Assembler &as);
    
    inline InputPort* frequencyPort() { return &_frequency_port; }
    
    inline InputPort* fmPort() { return &_fm_port; }
    
    inline OutputPort* valuePort() { return &_value_port; }
    
    inline OutputPort* normalizedPort() { return &_normalized_port; }
};
    
}//namespace r64fx

#endif//R64FX_MAIN_SAWTOOTH_OSC_H