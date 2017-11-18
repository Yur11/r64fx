#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_BufferRW : public SignalNode{
    float* m_buffer = nullptr;

public:
    SignalNode_BufferRW(SignalGraph &sg) : SignalNode(sg) {}

    inline void setBuffer(float* buffer) { m_buffer = buffer; }

    inline float* buffer() const { return m_buffer; }

    inline float& buffer(int i) { return m_buffer[i]; }
};


class SignalNode_BufferReader : public SignalNode_BufferRW{
    R64FX_NODE_SOURCE(out)

public:
    SignalNode_BufferReader(SignalGraph &sg) : SignalNode_BufferRW(sg), m_out(this) {}

private:
    virtual void build() override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW{
    R64FX_NODE_SINK(in)

public:
    SignalNode_BufferWriter(SignalGraph &sg) : SignalNode_BufferRW(sg) {}

private:
    virtual void build() override final;
};


}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
