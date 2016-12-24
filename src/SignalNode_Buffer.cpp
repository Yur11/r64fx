#include "SignalNode_Buffer.hpp"

namespace r64fx{

SignalNode_BufferReader::SignalNode_BufferReader(float* buffer)
: m_buffer(buffer)
, m_source(this)
{
    
}


void SignalNode_BufferReader::routine(int i)
{
    m_source[0] = m_buffer[i];
}


SignalNode_BufferWriter::SignalNode_BufferWriter(float* buffer)
: m_buffer(buffer)
, m_sink(this)
{
    
}


void SignalNode_BufferWriter::routine(int i)
{
    m_buffer[i] = m_sink[0];
}

}//namespace r64fx
