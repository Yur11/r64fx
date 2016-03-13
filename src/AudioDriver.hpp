#ifndef R64FX_AUDIO_DRIVER_HPP
#define R64FX_AUDIO_DRIVER_HPP

namespace r64fx{

class AudioIOPort{
public:
    virtual float* samples() = 0;

    virtual void lock() = 0;

    virtual bool tryLock() = 0;

    virtual void unlock() = 0;

    virtual bool isInput() = 0;

    virtual bool isGood() = 0;
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

    virtual AudioIOPort* newAudioInputPort(const char* name) = 0;

    virtual AudioIOPort* newAudioOutputPort(const char* name) = 0;

    virtual void deleteAudioPort(AudioIOPort* port) = 0;

    static AudioDriver* newInstance(AudioDriver::Type type = AudioDriver::Type::Jack);

    static void deleteInstance(AudioDriver* driver);
};


}//namespace r64fx

#endif//R64FX_AUDIO_DRIVER_HPP