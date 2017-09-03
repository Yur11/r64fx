#include "SignalNode_BufferRW.hpp"

namespace r64fx{

namespace{

void resize_buffer(float* &buffer, int new_size)
{
    if(buffer)
        delete[] buffer;
    if(new_size > 0)
        buffer = new float[new_size];
}

}//namespace


SignalGraphNode_BufferReader::SignalGraphNode_BufferReader()
: SignalGraphNode(1)
, m_source(this)
{

}


void SignalGraphNode_BufferReader::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalGraphNode_BufferReader::forEachPort(bool (*fun)(SignalGraphNode* node, SignalPort* port, void* arg), void* arg)
{
    fun(this, &m_source, arg);
}


void SignalGraphNode_BufferReader::prologue()
{

}


void SignalGraphNode_BufferReader::routine(int i)
{
    m_source[0] = m_buffer[i];
}


void SignalGraphNode_BufferReader::epilogue()
{

}


SignalGraphNode_BufferWriter::SignalGraphNode_BufferWriter()
: SignalGraphNode(1)
, m_sink(this)
{

}


void SignalGraphNode_BufferWriter::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalGraphNode_BufferWriter::forEachPort(bool (*fun)(SignalGraphNode* node, SignalPort* port, void* arg), void* arg)
{
    fun(this, &m_sink, arg);
}


void SignalGraphNode_BufferWriter::prologue()
{

}


void SignalGraphNode_BufferWriter::routine(int i)
{
    m_buffer[i] = m_sink[0];
}


void SignalGraphNode_BufferWriter::epilogue()
{

}

}//namespace r64fx
