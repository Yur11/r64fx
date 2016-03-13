#ifndef R64FX_AUDIO_DRIVER_HPP
#define R64FX_AUDIO_DRIVER_HPP

namespace r64fx{

class AudioDriver{
public:
    enum class Type{
        Jack
    };

    virtual ~AudioDriver() {};

    virtual bool isGood() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    static AudioDriver* newInstance(AudioDriver::Type type = AudioDriver::Type::Jack);

    static void deleteInstance(AudioDriver* driver);
};


}//namespace r64fx

#endif//R64FX_AUDIO_DRIVER_HPP