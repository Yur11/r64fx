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
{
}


void SignalNode_BufferReader::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalNode_BufferReader::build(SignalGraphProcessor &sgp)
{
    
}


SignalNode_BufferWriter::SignalNode_BufferWriter()
{
}


void SignalNode_BufferWriter::resizeBuffer(int new_size)
{
    resize_buffer(m_buffer, new_size);
}


void SignalNode_BufferWriter::build(SignalGraphProcessor &sgp)
{
    
}


}//namespace r64fx
