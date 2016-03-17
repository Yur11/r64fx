#ifndef R64FX_SOUND_DRIVER_HPP
#define R64FX_SOUND_DRIVER_HPP

#include <string>
#include "Midi.hpp"


namespace r64fx{

class SoundDriverIOPort{
    void* m = nullptr;

public:
    enum class Type{
        Bad,
        Audio,
        Midi
    };

    Type type() const;

    void setName(const std::string &name);

    std::string name() const;
};


class SoundDriverIOPort_Audio : public SoundDriverIOPort{
public:
    float* buffer() const;
};


class SoundDriverIOPort_Midi : public SoundDriverIOPort{
public:
    MidiEvent* events() const;

    int eventCount();
};


class SoundDriver{
public:
    enum class Type{
        Bad,
        Jack
    };

    virtual ~SoundDriver() {};

    virtual bool isGood() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

    virtual long count() = 0;

    static SoundDriver* newInstance(SoundDriver::Type type = SoundDriver::Type::Jack);

    static void deleteInstance(SoundDriver* driver);
};


}//namespace r64fx

#endif//R64FX_SOUND_DRIVER_HPP