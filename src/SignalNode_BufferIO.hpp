#ifndef R64FX_SIGNAL_NODE_BUFFER_IO_HPP
#define R64FX_SIGNAL_NODE_BUFFER_IO_HPP

#include "SignalGraph.hpp"
#include "SoundDriver.hpp"

namespace r64fx{
    
class SignalNode_BufferReader : public SignalNode{
    SoundDriverIOPort_AudioInput* m_input = nullptr;
    float* m_buffer = nullptr;
    int m_buffer_size = 0;
    SignalSource m_source;
    
public:
    SignalNode_BufferReader(SoundDriverIOPort_AudioInput* input, int buffer_size, SignalGraph* graph);
    
    virtual ~SignalNode_BufferReader();
    
    virtual void prepare();
    
    virtual void processSample(int i);
    
    inline SignalSource* source() { return &m_source; }
};


class SignalNode_BufferWriter : public SignalNode{
    SoundDriverIOPort_AudioOutput* m_output = nullptr;
    float* m_buffer = nullptr;
    int m_buffer_size = 0;
    SignalSink m_sink;
    
public:
    SignalNode_BufferWriter(SoundDriverIOPort_AudioOutput* output, int buffer_size, SignalGraph* graph);
    
    virtual ~SignalNode_BufferWriter();
    
    virtual void processSample(int i);
    
    virtual void finish();
    
    inline SignalSink* sink() { return &m_sink; }
};
    
};

#endif//R64FX_SIGNAL_NODE_BUFFER_IO_HPP