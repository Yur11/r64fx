#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_BufferRW : public SignalNode{
    DataBufferPointer m_buffer;
public:
    SignalNode_BufferRW(SignalGraph &sg, DataBufferPointer buffer) : SignalNode(sg), m_buffer(buffer) {}
    inline DataBufferPointer buffer() const { return m_buffer; }
};


class SignalNode_BufferReader : public SignalNode_BufferRW{
    R64FX_NODE_SOURCE(out)
public:
    SignalNode_BufferReader(SignalGraph &sg, DataBufferPointer buffer) : SignalNode_BufferRW(sg, buffer), m_out(this) {}
private:
    virtual void build() override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW{
    R64FX_NODE_SINK(in)
public:
    SignalNode_BufferWriter(SignalGraph &sg, DataBufferPointer buffer) : SignalNode_BufferRW(sg, buffer) {}
private:
    virtual void build() override final;
};


}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
