#include "SignalNode_BufferRW.hpp"
#include "SignalNodeFlags.hpp"

namespace r64fx{

void SignalNode_BufferReader::build()
{
    auto regs = allocRegisters<GPR64>(1);
    R64FX_DEBUG_ASSERT(regs);
    MOV(regs[0], ImmAddr(buffer() + frameCount()));
    MOV(regs[0], Base(regs[0]) + Index(rcx));
    initStorage<float, GPR64>(m_out, regs[0]);
}


void SignalNode_BufferWriter::build()
{
//     RegisterPack<GPR64> source_regs;
// 
//     auto source = m_in.connectedSource();
//     if(source)
//     {
//         ensureBuilt(source);
// 
//         if(source->isInRegisters())
//         {
//             source_regs = getStorageRegisters<GPR64>(*source);
//             removeStorageRegisters<GPR64>(*source);
//         }
//         else
//         {
//             source_regs = allocRegisters<GPR64>(1);
//             R64FX_DEBUG_ASSERT(source_regs);
//             MOV(source_regs[0], Mem64(addr(*source)));
//         }
//     }
//     else
//     {
//         source_regs = allocRegisters<GPR64>(1);
//         R64FX_DEBUG_ASSERT(source_regs);
//         XOR(source_regs[0], source_regs[0]);
//     }
// 
//     auto base_regs = allocRegisters<GPR64>(1);
//     R64FX_DEBUG_ASSERT(base_regs);
//     MOV(base_regs[0], ImmAddr(buffer() + frameCount()));
//     MOV(Base(base_regs[0]) + Index(rcx), source_regs[0]);
// 
//     freeRegisters(base_regs);
}

}//namespace r64fx
