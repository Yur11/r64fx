#ifndef R64FX_SIGNAL_NODE_AUDIO_IO_HPP
#define R64FX_SIGNAL_NODE_AUDIO_IO_HPP

#include "SignalNode.hpp"
#include <string>

namespace r64fx{

enum class SignalDirection{
    Input,
    Output
};


class SignalNodeClass_AudioIO : public SignalNodeClass{
protected:
    float**       m_buffers  = nullptr;
    int           m_size     = 0;

    SignalNodeClass_AudioIO(SignalGraph* parent_graph);

    virtual void nodeAppended(SignalNode* node);

    virtual void nodeRemoved(SignalNode* node);

    virtual SignalPort* port() = 0;

    void reallocateBuffers();

    void freeBuffers();

public:
    SignalNode* newNode(const std::string &name, int slot_count = 1);

     virtual SignalDirection direction() = 0;
};


class SignalNodeClass_AudioInput : public SignalNodeClass_AudioIO{
    SignalSource m_source;

public:
    SignalNodeClass_AudioInput(SignalGraph* parent_graph);

    virtual SignalDirection direction();

    inline SignalSource* source() { return &m_source; };

protected:
    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

    virtual SignalPort* port();
};


class SignalNodeClass_AudioOutput : public SignalNodeClass_AudioIO{
    SignalSink m_sink;

public:
    SignalNodeClass_AudioOutput(SignalGraph* parent_graph);

    virtual SignalDirection direction();

    inline SignalSink* sink() { return &m_sink; }

protected:
    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

    virtual SignalPort* port();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_AUDIO_IO_HPP