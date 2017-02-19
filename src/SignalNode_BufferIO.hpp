#ifndef R64FX_SIGNAL_NODE_BUFFER_IO_HPP
#define R64FX_SIGNAL_NODE_BUFFER_IO_HPP

#include "SignalGraph.hpp"
#include "SoundDriver.hpp"

namespace r64fx{


class SignalNode_BufferReader;

class BufferReaderSignalSource : public SignalSource{
    SignalNode_BufferReader* m_parent_reader = nullptr;

public:
    BufferReaderSignalSource(SignalNode_BufferReader* parent_reader, float* addr)
    : SignalSource(addr)
    , m_parent_reader(parent_reader)
    {}

    BufferReaderSignalSource()
    {}

    inline SignalNode_BufferReader* parentReader() const { return m_parent_reader; }
};


class SignalNode_BufferReader : public SignalNode{
    SoundDriverAudioInput* m_input = nullptr;
    float* m_buffer = nullptr;
    int m_buffer_size = 0;
    BufferReaderSignalSource m_source;

public:
    SignalNode_BufferReader(SoundDriverAudioInput* input, int buffer_size);

    virtual ~SignalNode_BufferReader();

    inline SoundDriverAudioInput* input() { return m_input; }

    inline BufferReaderSignalSource* source() { return &m_source; }

private:
    virtual void addedToGraph(SignalGraph* graph);

    virtual void aboutToBeRemovedFromGraph(SignalGraph* graph);

    virtual void prepare();

    virtual void processSample(int i);
};


class SignalNode_BufferWriter;

class BufferWriterSignalSink : public SignalSink{
    SignalNode_BufferWriter* m_parent_writer = nullptr;

public:
    BufferWriterSignalSink(SignalNode_BufferWriter* parent_writer, float* addr)
    : SignalSink(addr)
    , m_parent_writer(parent_writer)
    {}

    BufferWriterSignalSink()
    {}

    inline SignalNode_BufferWriter* parentWriter() const { return m_parent_writer; }
};


class SignalNode_BufferWriter : public SignalNode{
    SoundDriverAudioOutput* m_output = nullptr;
    float* m_buffer = nullptr;
    int m_buffer_size = 0;
    BufferWriterSignalSink m_sink;

public:
    SignalNode_BufferWriter(SoundDriverAudioOutput* output, int buffer_size);

    virtual ~SignalNode_BufferWriter();

    inline SoundDriverAudioOutput* output() { return m_output; }

    inline BufferWriterSignalSink* sink() { return &m_sink; }

private:
    virtual void addedToGraph(SignalGraph* graph);

    virtual void aboutToBeRemovedFromGraph(SignalGraph* graph);

    virtual void processSample(int i);

    virtual void finish();
};

};

#endif//R64FX_SIGNAL_NODE_BUFFER_IO_HPP
