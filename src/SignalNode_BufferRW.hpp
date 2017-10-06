#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNodeBuffer{
    float* m_buffer = nullptr;

public:
    inline void setBuffer(float* buffer) { m_buffer = buffer; }

    inline float* buffer() const { return m_buffer; }
};


class SignalNode_BufferReader : public SignalNode_WithSources<1>{

public:
    inline NodeSource source() { return {this, SignalNode_WithSources<1>::source(0)}; }

private:
    virtual void build(SignalGraphProcessor &sgp) override final;
};


class SignalNode_BufferWriter : public SignalNode_WithSinks<1>{

public:
    inline NodeSink sink() { return {this, SignalNode_WithSinks<1>::sink(0)}; }

private:
    virtual void build(SignalGraphProcessor &sgp) override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
