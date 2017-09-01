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


SignalNode_BufferReader::SignalNode_BufferReader()
: SignalNode(1)
, m_source(this)
{

}


void SignalNode_BufferReader::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalNode_BufferReader::forEachPort(bool (*fun)(SignalNode* node, SignalPort* port, void* arg), void* arg)
{
    fun(this, &m_source, arg);
}


void SignalNode_BufferReader::prologue()
{

}


void SignalNode_BufferReader::routine(int i)
{
    m_source[0] = m_buffer[i];
}


void SignalNode_BufferReader::epilogue()
{

}


SignalNode_BufferWriter::SignalNode_BufferWriter()
: SignalNode(1)
, m_sink(this)
{

}


void SignalNode_BufferWriter::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalNode_BufferWriter::forEachPort(bool (*fun)(SignalNode* node, SignalPort* port, void* arg), void* arg)
{
    fun(this, &m_sink, arg);
}


void SignalNode_BufferWriter::prologue()
{

}


void SignalNode_BufferWriter::routine(int i)
{
    m_buffer[i] = m_sink[0];
}


void SignalNode_BufferWriter::epilogue()
{

}

}//namespace r64fx
