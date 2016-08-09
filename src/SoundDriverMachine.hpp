#ifndef R64FX_SOUND_DRIVER_MACHINE_HPP
#define R64FX_SOUND_DRIVER_MACHINE_HPP

#include "Machine.hpp"
#include <string>

#include "Midi.hpp"

namespace r64fx{
    
class SoundDriverMachine : public Machine{
    LinkedList<MachinePort> m_ports;
    
public:
    SoundDriverMachine(MachinePool* pool);
    
    virtual ~SoundDriverMachine();
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
    void enable();
    
    void disable();
    
    void setSampleRate(int sample_rate);
    
    void setBufferSize(int buffer_size);
    
    void createAudioInput(const std::string &name);
    
    void createAudioOutput(const std::string &name);
    
    void createMidiInput(const std::string &name);
    
    void createMidiOutput(const std::string &name);
    
    void clear();
        
protected:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_MACHINE_HPP