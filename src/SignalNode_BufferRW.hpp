#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{


class SignalNode_BufferReader : public SignalNode_WithSources<1>{
    float* m_buffer = nullptr;

public:
    SignalNode_BufferReader();

    void resizeBuffer(int new_size);

    inline float* buffer() const { return m_buffer; }

    inline SignalSource* source() { return SignalNode_WithSources<1>::source(0); };

private:
    virtual void build(SignalGraphProcessor &sgp) override final;
};


class SignalNode_BufferWriter : public SignalNode_WithSinks<1>{
    SignalSink  m_sink;
    float*      m_buffer = nullptr;

public:
    SignalNode_BufferWriter();

    void resizeBuffer(int new_size);

    inline float* buffer() const { return m_buffer; }

    inline SignalSink* sink() { return SignalNode_WithSinks<1>::sink(0); }

private:
    virtual void build(SignalGraphProcessor &sgp) override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
