#ifndef R64FX_PLAYER_MACHINE_HPP
#define R64FX_PLAYER_MACHINE_HPP

#include "Machine.hpp"
#include "SoundFileDataPtr.hpp"

namespace r64fx{
    
class PlayerMachine : public Machine{
    MachineSignalSource m_output;
    SoundFileDataPtr m_sample;
    SoundFileDataPtr m_new_sample;
    
public:
    PlayerMachine(MachinePool* pool);
    
    virtual ~PlayerMachine();
    
    void replaceSample(SoundFileDataPtr data_ptr);
    
    void play();
    
    void stop();
    
    MachineSignalSource* output();
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
public:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_PLAYER_MACHINE_HPP