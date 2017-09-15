#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{


class SignalGraphNode_BufferReader : public SignalGraphNode_WithSources<1>{
    float* m_buffer = nullptr;

public:
    SignalGraphNode_BufferReader();

    void resizeBuffer(int new_size);

    inline float* buffer() const { return m_buffer; }

    inline SignalSource* source() { return SignalGraphNode_WithSources<1>::source(0); };

    virtual void process(SignalGraphProcessingContext* ctx) override final;
};


class SignalGraphNode_BufferWriter : public SignalGraphNode_WithSinks<1>{
    SignalSink  m_sink;
    float*      m_buffer = nullptr;

public:
    SignalGraphNode_BufferWriter();

    void resizeBuffer(int new_size);

    inline float* buffer() const { return m_buffer; }

    inline SignalSink* sink() { return SignalGraphNode_WithSinks<1>::sink(0); }

    virtual void process(SignalGraphProcessingContext* ctx) override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
