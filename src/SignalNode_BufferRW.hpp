#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

template<typename T, unsigned long Size> class SignalNode_BufferRW : public SignalNode{
    T* m_buffer[Size] = {nullptr, nullptr};

public:
    SignalNode_BufferRW(SignalGraph* sg) : SignalNode(sg) {}
    inline void setBuffer(unsigned long n, T* buffer) { m_buffer[n] = buffer; }
    inline T* buffer(unsigned long n) { return m_buffer[n]; }
};


class SignalNode_BufferReader : public SignalNode_BufferRW<float, 2>{
    R64FX_NODE_SOURCE(out)

public:
    SignalNode_BufferReader(SignalGraph* sg, float* buffer0, float* buffer1 = nullptr)
    : SignalNode_BufferRW<float, 2>(sg), m_out(this) { setBuffer(0, buffer0); setBuffer(1, buffer1); }

private:
    virtual void build() override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW<float, 2>{
    R64FX_NODE_SINK(in)

public:
    SignalNode_BufferWriter(SignalGraph* sg, float* buffer0, float* buffer1 = nullptr)
    : SignalNode_BufferRW<float, 2>(sg) { setBuffer(0, buffer0); setBuffer(1, buffer1); }

private:
    virtual void build() override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
