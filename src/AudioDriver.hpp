#ifndef R64FX_AUDIO_DRIVER_HPP
#define R64FX_AUDIO_DRIVER_HPP

#include "Midi.hpp"
#include "Mutex.hpp"


namespace r64fx{

class AudioIOPort : public Mutex{
public:
    virtual float* samples() = 0;

    virtual bool isInput() = 0;

    virtual bool isGood() = 0;
};


class MidiIOPort : public Mutex{
public:
    virtual MidiEvent* event(int i) = 0;

    virtual int eventCount() = 0;
};


class AudioDriver : public Mutex{
public:
    enum class Type{
        Jack
    };

    virtual ~AudioDriver() {};

    virtual bool isGood() = 0;

    virtual unsigned long count() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    virtual int bufferSize() = 0;

    virtual int sampleRate() = 0;

    virtual AudioIOPort* newAudioInputPort(const char* name) = 0;

    virtual AudioIOPort* newAudioOutputPort(const char* name) = 0;

    virtual void deleteAudioPort(AudioIOPort* port) = 0;

    virtual MidiIOPort* newMidiInputPort(const char* name) = 0;

    virtual MidiIOPort* newMidiOutputPort(const char* name) = 0;

    virtual void deleteMidiPort(MidiIOPort* port) = 0;

    static AudioDriver* newInstance(AudioDriver::Type type = AudioDriver::Type::Jack);

    static void deleteInstance(AudioDriver* driver);
};


}//namespace r64fx

#endif//R64FX_AUDIO_DRIVER_HPP