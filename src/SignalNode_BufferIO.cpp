#include "SignalNode_BufferIO.hpp"

namespace r64fx{
    
SignalNode_BufferReader::SignalNode_BufferReader(SoundDriverIOPort_AudioInput* input, int buffer_size, SignalGraph* graph)
{
    m_input = input;
    m_buffer = new float[buffer_size];
    m_buffer_size = buffer_size;
    m_source = SignalSource(new float);
    setGraph(graph);
    graph->addNode(this);
}


SignalNode_BufferReader::~SignalNode_BufferReader()
{
    delete[] m_buffer;
    delete m_source.addr();
    graph()->removeNode(this);
}
    
    
void SignalNode_BufferReader::prepare()
{
    int nsamples = m_input->readSamples(m_buffer, m_buffer_size);
    for(int i=nsamples; i<m_buffer_size; i++)
    {
        m_buffer[i] = 0.0f;
    }
}


void SignalNode_BufferReader::processSample(int i)
{
    m_source[0] = m_buffer[i];
}



SignalNode_BufferWriter::SignalNode_BufferWriter(SoundDriverIOPort_AudioOutput* output, int buffer_size, SignalGraph* graph)
{
    m_output = output;
    m_buffer = new float[buffer_size];
    m_buffer_size = buffer_size;
    m_sink = SignalSink(new float);
    setGraph(graph);
    graph->addNode(this);
}
    
    
SignalNode_BufferWriter::~SignalNode_BufferWriter()
{
    delete[] m_buffer;
    delete m_sink.addr();
    graph()->removeNode(this);
}


void SignalNode_BufferWriter::processSample(int i)
{
    m_buffer[i] = m_sink[0];
}


void SignalNode_BufferWriter::finish()
{
    m_output->writeSamples(m_buffer, m_buffer_size);
}
    
}//namespace r64fx