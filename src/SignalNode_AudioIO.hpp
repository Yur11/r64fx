#ifndef R64FX_SIGNAL_NODE_AUDIO_IO_HPP
#define R64FX_SIGNAL_NODE_AUDIO_IO_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_AudioIO : public SignalNodeClass{
protected:
    SoundDriver*  m_driver   = nullptr;
    float*        m_ports    = nullptr;
    float**       m_buffers  = nullptr;
    int           m_size     = 0;

    SignalNodeClass_AudioIO(SoundDriver* driver);

    virtual void nodeAppended(SignalNode* node);

    virtual void nodeRemoved(SignalNode* node);

    void reallocateBuffers();

    void freeBuffers();

public:
    virtual void prepare();
};


class SignalNodeClass_AudioInput : public SignalNodeClass_AudioIO{
public:
    using SignalNodeClass_AudioIO::SignalNodeClass_AudioIO;

    virtual void process(int sample);

    virtual void finish();
};


class SignalNodeClass_AudioOutput : public SignalNodeClass_AudioIO{
public:
    using SignalNodeClass_AudioIO::SignalNodeClass_AudioIO;

    virtual void process(int sample);

    virtual void finish();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_AUDIO_IO_HPP