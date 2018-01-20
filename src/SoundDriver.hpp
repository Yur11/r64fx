#ifndef R64FX_SOUND_DRIVER_HPP
#define R64FX_SOUND_DRIVER_HPP

#include <string>
#include "Midi.hpp"
#include "CircularBuffer.hpp"

namespace r64fx{

/* Opaque handles for sound driver ports. */
class SoundDriverPort{ SoundDriverPort(){} SoundDriverPort(const SoundDriverPort&){} };

class SoundDriverAudioPort    : public SoundDriverPort{};
class SoundDriverAudioInput   : public SoundDriverAudioPort{};
class SoundDriverAudioOutput  : public SoundDriverAudioPort{};
class SoundDriverMidiPort     : public SoundDriverPort{};
class SoundDriverMidiInput    : public SoundDriverMidiPort{};
class SoundDriverMidiOutput   : public SoundDriverMidiPort{};


/* Used for communication and synchronization between sound driver and worker thread. 
   Each worker thread must use a separate instance. */
class SoundDriverPortGroup{
    void*                           m_impl                     = nullptr;
    CircularBuffer<unsigned long>*  m_to_impl                  = nullptr;
    CircularBuffer<unsigned long>*  m_from_impl                = nullptr;
    unsigned long                   m_sync_value               = 0;

    SoundDriverPortGroup();

    typedef void (AudioPortUpdateCallback)(SoundDriverAudioPort*, float* buffer, void* arg);
    typedef void (MidiPortUpdateCallback)(SoundDriverMidiPort*, MidiEventBuffer* buffer, void* arg);
    typedef void (PortUpdateCallback)(SoundDriverPort*, void* buffer, void* arg);

    void updatePort(
        SoundDriverPort* port, void* buffer, PortUpdateCallback* callback = nullptr, void* arg = nullptr
    );

public:
    inline void updatePort(
        SoundDriverAudioPort* port, float* buffer, AudioPortUpdateCallback* callback = nullptr, void* arg = nullptr
    )
    { updatePort((SoundDriverPort*)port, (void*)buffer, (PortUpdateCallback*)callback, (void*)arg); }

    inline void updatePort(
        SoundDriverMidiPort* port, MidiEventBuffer* buffer, MidiPortUpdateCallback* callback = nullptr, void* arg = nullptr
    )
    { updatePort((SoundDriverPort*)port, (void*)buffer, (PortUpdateCallback*)callback, (void*)arg); }

    /* Enable/Disable sync/done mechanism for this port group.*/
    void enable();
    void disable();

    /* Wait for sync() to return true. Update your ports. Call done() . */
    bool sync();
    void done();

    inline bool isGood() const { return m_impl; }

    friend class SoundDriver;
};


class SoundDriver{
protected:
    CircularBuffer<unsigned long>  m_to_impl;
    CircularBuffer<unsigned long>  m_from_impl;
    unsigned long                  m_port_group_count = 0;

public:
    SoundDriver();

    virtual ~SoundDriver();

    enum class Type{
        Default,
        Stub,
        Jack
    };

    virtual SoundDriver::Type type() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

    SoundDriverPortGroup* newPortGroup();

    void deletePortGroup(SoundDriverPortGroup* port_group);

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name) = 0;

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name) = 0;

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name) = 0;

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name) = 0;

    inline bool isClean() const { return m_port_group_count == 0; }

    void sync();

    inline void newPort(SoundDriverAudioInput** port, const std::string &name)
        { *port = newAudioInput(name); }

    inline void newPort(SoundDriverAudioOutput** port, const std::string &name)
        { *port = newAudioOutput(name); }

    inline void newPort(SoundDriverMidiInput** port, const std::string &name)
        { *port = newMidiInput(name); }

    inline void newPort(SoundDriverMidiOutput** port, const std::string &name)
        { *port = newMidiOutput(name); }

    virtual void deletePort(SoundDriverPort* port) = 0;

    virtual void setPortName(SoundDriverPort* port, const std::string &name) = 0;

    virtual void getPortName(SoundDriverPort* port, std::string &name) = 0;

    virtual bool connect(const std::string &src, const std::string &dst) = 0;

    virtual bool disconnect(const std::string &src, const std::string &dst) = 0;

    static SoundDriver* newInstance(SoundDriver::Type type, const char* client_name);

    static void deleteInstance(SoundDriver* driver);
};

}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_HPP
