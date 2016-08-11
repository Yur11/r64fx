#ifndef R64FX_PLAYER_MACHINE_HPP
#define R64FX_PLAYER_MACHINE_HPP

#include "Machine.hpp"
#include "SignalData.hpp"

namespace r64fx{
    
class PlayerMachine : public Machine{
    MachineSignalSource m_output;
    SignalData* m_data = nullptr;
    
public:
    PlayerMachine(MachinePool* pool);
    
    virtual ~PlayerMachine();
    
    void setData(SignalData* data);
    
    void play();
    
    void stop();
    
    MachineSignalSource* output();
    
    virtual void forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg);
    
public:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_PLAYER_MACHINE_HPP