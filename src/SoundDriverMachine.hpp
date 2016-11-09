#ifndef R64FX_SOUND_DRIVER_MACHINE_HPP
#define R64FX_SOUND_DRIVER_MACHINE_HPP

#include "Machine.hpp"
#include <string>

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
    
    MachineSignalSource* createAudioInput(const std::string &name, int component_count);
    
    MachineSignalSink* createAudioOutput(const std::string &name, int component_count);
    
    void createMidiInput(const std::string &name);
    
    void createMidiOutput(const std::string &name);
    
    void destroyPort(MachinePort* port);
    
    void connect(const std::string &src, const std::string &dst);

    void disconnect(const std::string &src, const std::string &dst);
       
protected:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_MACHINE_HPP
