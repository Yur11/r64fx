#include "SignalNode_BufferRW.hpp"
#include "SignalNodeFlags.hpp"

namespace r64fx{

float** SignalNode_BufferRW::storeBufferLocation(SignalGraphCompiler &c)
{
    if(!m_buff_ptr_addr)
    {
        c.allocStorage(m_buff_ptr_addr, SignalDataStorage::Addr(), 1, 8);
    }

    auto ptr = (float**) m_buff_ptr_addr.memoryAddr(c);
    ptr[0] = m_buffer;
    return ptr;
}


void SignalNode_BufferReader::build(SignalGraphCompiler &c)
{
    if(m_out.connectedSinkCount() == 0)
        return;

    storeBufferLocation(c);

    for(;;)//Handle reallocations.
    {
        auto ce = c.codeEnd();
        auto bu = c.codeBytesUsed();
        c.MOV(rax, ImmAddr(storeBufferLocation(c)));
        c.MOV(rax, Base(rax));
        if(ce == c.codeEnd())
            break;
        c.setCodeEnd(c.codeBegin() + bu);
    }

    if(!m_out)
    {
        c.allocStorage(m_out, SignalDataStorage::Memory() | SignalDataStorage::Single() | SignalDataStorage::Float(), 1, 4);
//         c.MOV(Mem32(m_out.memoryAddr(c)), rax);
    }
}


void SignalNode_BufferWriter::build(SignalGraphCompiler &c)
{
    storeBufferLocation(c);
}


}//namespace r64fx
