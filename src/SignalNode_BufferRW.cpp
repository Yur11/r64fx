#include "SignalNode_BufferRW.hpp"
#include "SignalGraphProcessingContext.hpp"

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
{
}


void SignalGraphNode_BufferReader::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalGraphNode_BufferReader::process(SignalGraphProcessingContext* ctx)
{
    
}


SignalGraphNode_BufferWriter::SignalGraphNode_BufferWriter()
{
}


void SignalGraphNode_BufferWriter::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalGraphNode_BufferWriter::process(SignalGraphProcessingContext* ctx)
{
    
}


}//namespace r64fx
