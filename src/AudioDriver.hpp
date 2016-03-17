#ifndef R64FX_AUDIO_DRIVER_HPP
#define R64FX_AUDIO_DRIVER_HPP

#include <string>
#include "Midi.hpp"


namespace r64fx{

class AudioDriverIOPort{
    void* m = nullptr;

public:
    virtual void setName(const std::string &name) = 0;

    virtual std::string name() = 0;
};


class AudioDriver{
public:
    enum class Type{
        Jack
    };

    virtual ~AudioDriver() {};

    virtual bool isGood() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

//     virtual AudioIOPort* newAudioInputPort(const char* name) = 0;

//     virtual AudioIOPort* newAudioOutputPort(const char* name) = 0;

//     virtual void deleteAudioPort(AudioIOPort* port) = 0;

//     virtual MidiIOPort* newMidiInputPort(const char* name) = 0;

//     virtual MidiIOPort* newMidiOutputPort(const char* name) = 0;

//     virtual void deleteMidiPort(MidiIOPort* port) = 0;

    static AudioDriver* newInstance(AudioDriver::Type type = AudioDriver::Type::Jack);

    static void deleteInstance(AudioDriver* driver);
};


}//namespace r64fx

#endif//R64FX_AUDIO_DRIVER_HPP