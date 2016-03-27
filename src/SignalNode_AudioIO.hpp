#ifndef R64FX_SIGNAL_NODE_AUDIO_IO_HPP
#define R64FX_SIGNAL_NODE_AUDIO_IO_HPP

#include "SignalNode.hpp"

namespace r64fx{

class SignalNodeClass_AudioIO : public SignalNodeClass{

};


class SignalNodeClass_AudioInput : public SignalNodeClass{
public:
    virtual void prepare();

    virtual void process();
};


class SignalNodeClass_AudioOutput : public SignalNodeClass{
public:
    virtual void prepare();

    virtual void process();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_AUDIO_IO_HPP