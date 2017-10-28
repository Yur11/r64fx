#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_BufferRW : public SignalNode{
    float* m_buffer = nullptr;

public:
    inline void setBuffer(float* buffer) { m_buffer = buffer; }

    inline float* buffer() const { return m_buffer; }

    inline float& buffer(int i) { return m_buffer[i]; }
};


class SignalNode_BufferReader : public SignalNode_BufferRW{
    R64FX_NODE_SOURCE(out)

public:
    SignalNode_BufferReader() : m_out(this) {}

private:
    virtual void build(SignalGraphCompiler &c) override final;

    virtual void cleanup(SignalGraphCompiler &c) override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW{
    R64FX_NODE_SINK(in)

    virtual void build(SignalGraphCompiler &c) override final;

    virtual void cleanup(SignalGraphCompiler &c) override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
