#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_BufferRW : public SignalNode{
    float* m_buffer = nullptr;
    SignalDataStorage m_buff_ptr_addr;

public:
    inline void setBuffer(float* buffer) { m_buffer = buffer; }

    inline float* buffer() const { return m_buffer; }

    inline float& buffer(int i) { return m_buffer[i]; }

protected:
    float** storeBufferLocation(SignalGraphCompiler &c);
};


class SignalNode_BufferReader : public SignalNode_BufferRW{
    SignalSource m_out;

public:
    inline NodeSource out() { return {this, &m_out}; }

private:
    virtual void build(SignalGraphCompiler &c) override final;
};


class SignalNode_BufferWriter : public SignalNode_BufferRW{
    SignalSink m_in;

public:
    inline NodeSink in() { return {this, &m_in}; }

private:
    virtual void build(SignalGraphCompiler &c) override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
