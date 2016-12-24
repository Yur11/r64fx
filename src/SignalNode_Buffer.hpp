#ifndef R64FX_SIGNAL_NODE_BUFFER_HPP
#define R64FX_SIGNAL_NODE_BUFFER_HPP

#include "SignalGraph.hpp"

namespace r64fx{


class SignalNode_BufferReader : public SignalNode{
    float* m_buffer = nullptr;
    SignalSource m_source;

public:
    SignalNode_BufferReader(float* buffer);

private:
    virtual void routine(int i);
};


class SignalNode_BufferWriter : public SignalNode{
    float* m_buffer = nullptr;
    SignalSink m_sink;

public:
    SignalNode_BufferWriter(float* buffer);

private:
    virtual void routine(int i);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_HPP
