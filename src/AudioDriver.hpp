#ifndef R64FX_AUDIO_DRIVER_HPP
#define R64FX_AUDIO_DRIVER_HPP

#include <string>
#include "Midi.hpp"


namespace r64fx{

class AudioDriverIOPort{
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


class AudioDriverIOPort_Audio : public AudioDriverIOPort{
public:
    float* buffer() const;
};


class AudioDriverIOPort_Midi : public AudioDriverIOPort{
public:
    MidiEvent* events() const;

    int eventCount();
};


class AudioDriver{
public:
    enum class Type{
        Bad,
        Jack
    };

    virtual ~AudioDriver() {};

    virtual bool isGood() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

    static AudioDriver* newInstance(AudioDriver::Type type = AudioDriver::Type::Jack);

    static void deleteInstance(AudioDriver* driver);
};


}//namespace r64fx

#endif//R64FX_AUDIO_DRIVER_HPP