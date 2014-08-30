#ifndef R64FX_MAIN_MONOPHONIC_MIXER_H
#define R64FX_MAIN_MONOPHONIC_MIXER_H

#include "dsp/Processor.hpp"

namespace r64fx{
    
class MonophonicMixer : public Processor{
    InputPort _input_port;
    InputPort _level_port;
    float* _mix;
    
public:
    MonophonicMixer();
    
   ~MonophonicMixer();
   
    virtual void render(Assembler &as);
    
    inline InputPort* inputPort() { return &_input_port; }
    
    inline InputPort* levelPort() { return &_level_port; }
    
    inline float* mix() { return _mix; }
};
    
}//namespace r64fx

#endif//R64FX_MAIN_MONOPHONIC_MIXER_H