#ifndef R64FX_SOUND_DRIVER_HPP
#define R64FX_SOUND_DRIVER_HPP

#include <string>
#include "Midi.hpp"


namespace r64fx{

class SoundDriverIOPort{
public:
    enum class Type{
        Bad,
        Audio,
        Midi
    };

    virtual Type type() = 0;

    enum class Direction{
        Input,
        Output
    };

    virtual Direction direction() = 0;

    virtual void setName(const std::string &name) = 0;

    virtual std::string name() = 0;
};


class SoundDriverIOPort_Audio : public SoundDriverIOPort{

};

class SoundDriverIOPort_AudioInput   : public SoundDriverIOPort_Audio{

};

class SoundDriverIOPort_AudioOutput  : public SoundDriverIOPort_Audio{

};


class SoundDriverIOPort_Midi : public SoundDriverIOPort{

};

class SoundDriverIOPort_MidiInput    : public SoundDriverIOPort_Midi{
public:
    virtual int readEvents(MidiEvent* out, int nevents) = 0;
};

class SoundDriverIOPort_MidiOutput   : public SoundDriverIOPort_Midi{

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

    virtual long count() = 0;

    virtual SoundDriverIOPort_MidiInput* newMidiInput(const std::string &name = "") = 0;

    static SoundDriver* newInstance(SoundDriver::Type type = SoundDriver::Type::Jack);

    static void deleteInstance(SoundDriver* driver);
};


}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_HPP