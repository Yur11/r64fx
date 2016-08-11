#ifndef R64FX_PLAYER_MACHINE_HPP
#define R64FX_PLAYER_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{
    
class PlayerMachine : public Machine{
    MachineSignalSource m_signal_source;
    
public:
    PlayerMachine(MachinePool* pool);
    
    virtual ~PlayerMachine();
    
    void playData(float* data, int frame_count, int component_count);
    
    MachineSignalSource* signalSource();
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
public:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_PLAYER_MACHINE_HPP