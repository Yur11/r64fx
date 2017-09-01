#ifndef R64FX_SIGNAL_NODE_BUFFER_RW_HPP
#define R64FX_SIGNAL_NODE_BUFFER_RW_HPP

#include "SignalGraph.hpp"

namespace r64fx{


class SignalNode_BufferReader : public SignalNode{
    SignalSource  m_source;
    float*        m_buffer = nullptr;

public:
    SignalNode_BufferReader();

    void resizeBuffer(int new_size);

    inline float* buffer() const { return m_buffer; }

    inline SignalSource* source() { return &m_source; }

    virtual void forEachPort(bool (*fun)(SignalNode* node, SignalPort* port, void* arg), void* arg) override final;

private:
    virtual void prologue() override final;

    virtual void routine(int i) override final;

    virtual void epilogue() override final;
};


class SignalNode_BufferWriter : public SignalNode{
    SignalSink  m_sink;
    float*      m_buffer = nullptr;

public:
    SignalNode_BufferWriter();

    void resizeBuffer(int new_size);

    inline float* buffer() const { return m_buffer; }

    inline SignalSink* sink() { return &m_sink; }

    virtual void forEachPort(bool (*fun)(SignalNode* node, SignalPort* port, void* arg), void* arg)  override final;

private:
    virtual void prologue() override final;

    virtual void routine(int i) override final;

    virtual void epilogue() override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_BUFFER_RW_HPP
