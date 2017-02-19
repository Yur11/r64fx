#ifndef R64FX_PLAYER_MACHINE_HPP
#define R64FX_PLAYER_MACHINE_HPP

#include "MachineIface.hpp"
#include "SoundFileDataPtr.hpp"

namespace r64fx{

class PlayerMachine : public MachineIface{
    MachineSignalSource m_output;
    SoundFileDataPtr m_sample;
    SoundFileDataPtr m_new_sample;
    float m_pitch  = 1.0f;
    float m_gain   = 1.0f;
    float m_playhead_time = 0.0f;
    bool  m_is_playing = false;

public:
    PlayerMachine(MachinePool* pool);

    virtual ~PlayerMachine();

    void replaceSample(SoundFileDataPtr data_ptr);

    void play();

    void stop();

    void setPitch(float pitch);

    float pitch() const;

    void setGain(float gain);

    float gain() const;

    void setPlayheadTime(float playhead_time);

    float playheadTime() const;

    bool isPlaying() const;

    MachineSignalSource* output();

    virtual void forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg);

public:


    virtual void messageRecievedFromImpl(const MachineMessage &msg);
};

}//namespace r64fx

#endif//R64FX_PLAYER_MACHINE_HPP
