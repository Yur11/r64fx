#ifndef R64FX_SOUND_DRIVER_HPP
#define R64FX_SOUND_DRIVER_HPP

#include <string>
#include "Midi.hpp"


namespace r64fx{

class SoundDriverIOPort{
public:
    enum class Type{
        Audio,
        Midi
    };

    virtual Type type() = 0;

    enum class Direction{
        Input,
        Output
    };

    virtual ~SoundDriverIOPort() {}

    virtual Direction direction() = 0;

    virtual void setName(const std::string &name) = 0;

    virtual std::string name() = 0;
};


class SoundDriverIOPort_Audio : public SoundDriverIOPort{

};

class SoundDriverIOPort_AudioInput   : public SoundDriverIOPort_Audio{
public:
    virtual int readSamples(float* samples, int nsamples) = 0;
};

class SoundDriverIOPort_AudioOutput  : public SoundDriverIOPort_Audio{
public:
    virtual int writeSamples(float* samples, int nsamples) = 0;
};


class SoundDriverIOPort_Midi : public SoundDriverIOPort{

};

class SoundDriverIOPort_MidiInput    : public SoundDriverIOPort_Midi{
public:
    virtual int readEvents(MidiEvent* events, int nevents) = 0;
};

class SoundDriverIOPort_MidiOutput   : public SoundDriverIOPort_Midi{
public:
    virtual int writeEvents(MidiEvent* events, int nevents) = 0;
};


struct SoundDriverIOStatus{
    long begin_time = 0;
    long end_time = 0;

    inline operator bool()
    {
        return begin_time != 0 && end_time != 0;
    }
};

class SoundDriverIOStatusPort{
public:
    virtual int readStatus(SoundDriverIOStatus* status, int nitems = 1) = 0;
};


class SoundDriver{
public:
    enum class Type{
        Bad,
        Jack
    };

    virtual ~SoundDriver() {};

    virtual SoundDriver::Type type() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

    virtual SoundDriverIOPort_AudioInput* newAudioInput(const std::string &name = "") = 0;

    virtual SoundDriverIOPort_AudioOutput* newAudioOutput(const std::string &name = "") = 0;

    virtual SoundDriverIOPort_MidiInput* newMidiInput(const std::string &name = "") = 0;

    virtual SoundDriverIOPort_MidiOutput* newMidiOutput(const std::string &name = "") = 0;
    
    virtual SoundDriverIOPort* findPort(const std::string &name) = 0;
    
    virtual void deletePort(SoundDriverIOPort* port) = 0;

    virtual SoundDriverIOStatusPort* newStatusPort() = 0;

    static SoundDriver* newInstance(SoundDriver::Type type = SoundDriver::Type::Jack);

    static void deleteInstance(SoundDriver* driver);
};


}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_HPP