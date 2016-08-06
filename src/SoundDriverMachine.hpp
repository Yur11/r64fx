#ifndef R64FX_SOUND_DRIVER_MACHINE_HPP
#define R64FX_SOUND_DRIVER_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{
    
class SoundDriverMachine : public Machine{
public:
    SoundDriverMachine(MachinePool* pool);
    
    virtual ~SoundDriverMachine();
    
    void enable();
    
    void disable();
    
    void setSampleRate(int sample_rate);
    
    void setBufferSize(int buffer_size);
    
protected:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_MACHINE_HPP