#ifndef R64FX_SOUND_DRIVER_HPP
#define R64FX_SOUND_DRIVER_HPP

#include <string>
#include "Midi.hpp"


namespace r64fx{

class SoundDriverPort{
public:
    virtual ~SoundDriverPort() {}

    enum class Type{
        Audio,
        Midi
    };

    virtual Type type() = 0;

    enum class Direction{
        Input,
        Output
    };

    virtual Direction direction() = 0;
};


class SoundDriverAudioPort : public SoundDriverPort{

};

class SoundDriverAudioInput : public SoundDriverAudioPort{
public:
    virtual int readSamples(float* samples, int nsamples) = 0;
};

class SoundDriverAudioOutput : public SoundDriverAudioPort{
public:
    virtual int writeSamples(float* samples, int nsamples) = 0;
};


class SoundDriverMidiPort : public SoundDriverPort{

};

class SoundDriverMidiInput : public SoundDriverMidiPort{
public:
    virtual int readEvents(MidiEvent* events, int nevents) = 0;
};

class SoundDriverMidiOutput : public SoundDriverMidiPort{
public:
    virtual int writeEvents(MidiEvent* events, int nevents) = 0;
};


struct SoundDriverSyncMessage{
    unsigned long bits = 0;

    SoundDriverSyncMessage() {}

    SoundDriverSyncMessage(unsigned long bits) : bits(bits) {}
};


class SoundDriverSyncPort{
public:
    virtual ~SoundDriverSyncPort(){}

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int readMessages(SoundDriverSyncMessage* msgs, int nmsgs) = 0;
};


class SoundDriver{
public:
    enum class Type{
        Bad,
        Stub,
        Jack
    };

    virtual ~SoundDriver() {};

    virtual SoundDriver::Type type() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name = "") = 0;

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name = "") = 0;

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name = "") = 0;

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name = "") = 0;

    virtual void deletePort(SoundDriverPort* port) = 0;

    virtual SoundDriverSyncPort* newSyncPort() = 0;

    virtual void deleteSyncPort(SoundDriverSyncPort* port) = 0;

    virtual void setPortName(SoundDriverPort* port, const std::string &name) = 0;

    virtual void getPortName(SoundDriverPort* port, std::string &name) = 0;

    virtual bool connect(const std::string &src, const std::string &dst) = 0;

    virtual bool disconnect(const std::string &src, const std::string &dst) = 0;

    virtual bool hasPorts() = 0;

    virtual void processEvents() = 0;

    static SoundDriver* newInstance(SoundDriver::Type type);

    static void deleteInstance(SoundDriver* driver);
};


}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_HPP
