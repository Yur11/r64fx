#ifndef R64FX_SIGNAL_NODE_AUDIO_IO_HPP
#define R64FX_SIGNAL_NODE_AUDIO_IO_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_AudioIO : public SignalNodeClass{
protected:
    SoundDriver*  m_driver   = nullptr;
    float**       m_buffers  = nullptr;
    int           m_size     = 0;

    SignalNodeClass_AudioIO(SoundDriver* driver);

    virtual void nodeAppended(SignalNode* node);

    virtual void nodeRemoved(SignalNode* node);

    virtual SignalPort* port() = 0;

    void reallocateBuffers();

    void freeBuffers();

public:
    SignalNode* newNode(const std::string &name, int slot_count = 1);

    virtual SoundDriverIOPort::Direction direction() = 0;
};


class SignalNodeClass_AudioInput : public SignalNodeClass_AudioIO{
    SignalSource m_source;

public:
    SignalNodeClass_AudioInput(SoundDriver* driver);

    virtual SoundDriverIOPort::Direction direction();

    inline SignalSource* source() { return &m_source; };

    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

protected:
    virtual SignalPort* port();
};


class SignalNodeClass_AudioOutput : public SignalNodeClass_AudioIO{
    SignalSink m_sink;

public:
    SignalNodeClass_AudioOutput(SoundDriver* driver);

    virtual SoundDriverIOPort::Direction direction();

    inline SignalSink* sink() { return &m_sink; }

    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

protected:
    virtual SignalPort* port();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_AUDIO_IO_HPP