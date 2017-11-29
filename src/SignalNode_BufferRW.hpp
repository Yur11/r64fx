#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_BufferRW : public SignalNode{
    float* m_buffer = nullptr;

public:
    SignalNode_BufferRW(SignalGraph &sg, float* buffer) : SignalNode(sg), m_buffer(buffer) {}

    inline float* buffer() const { return m_buffer; }
};


class SignalNode_BufferReader : public SignalNode_BufferRW{
    R64FX_NODE_SOURCE(out)

public:
    SignalNode_BufferReader(SignalGraph &sg, float* buffer) : SignalNode_BufferRW(sg, buffer), m_out(this) {}

private:
    virtual void build() override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW{
    R64FX_NODE_SINK(in)

public:
    SignalNode_BufferWriter(SignalGraph &sg, float* buffer) : SignalNode_BufferRW(sg, buffer) {}

private:
    virtual void build() override final;
};


}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
