#include "SignalNode_BufferIO.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
SignalNode_BufferReader::SignalNode_BufferReader(SoundDriverAudioInput* input, int buffer_size)
{
    m_input = input;
    m_buffer = new float[buffer_size];
    for(int i=0; i<buffer_size; i++)
    {
        m_buffer[i] = 0.0f;
    }
    m_buffer_size = buffer_size;
    m_source = BufferReaderSignalSource(this, new float);
    m_source[0] = 0.0f;
}


SignalNode_BufferReader::~SignalNode_BufferReader()
{
    delete[] m_buffer;
    delete m_source.addr();
    removeFromGraph();
}
    
    
void SignalNode_BufferReader::addedToGraph(SignalGraph* graph)
{
    
}
    
    
void SignalNode_BufferReader::aboutToBeRemovedFromGraph(SignalGraph* graph)
{
    
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



SignalNode_BufferWriter::SignalNode_BufferWriter(SoundDriverAudioOutput* output, int buffer_size)
{
    m_output = output;
    m_buffer = new float[buffer_size];
    for(int i=0; i<buffer_size; i++)
    {
        m_buffer[i] = 0.0f;
    }
    m_buffer_size = buffer_size;
    m_sink = BufferWriterSignalSink(this, new float);
    m_sink[0] = 0.0f;
}
    
    
SignalNode_BufferWriter::~SignalNode_BufferWriter()
{
    delete[] m_buffer;
    delete m_sink.addr();
    removeFromGraph();
}


void SignalNode_BufferWriter::addedToGraph(SignalGraph* graph)
{
    
}
    
    
void SignalNode_BufferWriter::aboutToBeRemovedFromGraph(SignalGraph* graph)
{
    
}


void SignalNode_BufferWriter::processSample(int i)
{
//     cout << i << " writer " << m_sink[0] << "\n";
    m_buffer[i] = m_sink[0];
}


void SignalNode_BufferWriter::finish()
{
    m_output->writeSamples(m_buffer, m_buffer_size);
}
    
}//namespace r64fx
