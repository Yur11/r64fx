#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_BufferRW : public SignalNode{
    DataBufferPointer m_buffer0;
    DataBufferPointer m_buffer1;

public:
    SignalNode_BufferRW(SignalGraph &sg, DataBufferPointer buffer0, DataBufferPointer buffer1 = {})
    : SignalNode(sg), m_buffer0(buffer0), m_buffer1(buffer1) {}

    inline void setBuffer1(DataBufferPointer buffer) { m_buffer0 = buffer; }
    inline void setBuffer2(DataBufferPointer buffer) { m_buffer1 = buffer; }

    inline DataBufferPointer buffer0() const { return m_buffer0; }
    inline DataBufferPointer buffer1() const { return m_buffer1; }

    inline float* buffer0Addr() const { return addr<float>(m_buffer0); }
    inline float* buffer1Addr() const { return addr<float>(m_buffer1); }
};


class SignalNode_BufferReader : public SignalNode_BufferRW{
    R64FX_NODE_SOURCE(out)

public:
    SignalNode_BufferReader(SignalGraph &sg, DataBufferPointer buffer0, DataBufferPointer buffer1 = {})
    : SignalNode_BufferRW(sg, buffer0, buffer1), m_out(this) {}

private:
    virtual void build() override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW{
    R64FX_NODE_SINK(in)

public:
    SignalNode_BufferWriter(SignalGraph &sg, DataBufferPointer buffer0, DataBufferPointer buffer1 = {})
    : SignalNode_BufferRW(sg, buffer0, buffer1) {}

private:
    virtual void build() override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
