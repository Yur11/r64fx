#include "SignalNode_BufferRW.hpp"
#include "SignalNodeFlags.hpp"

namespace r64fx{

void SignalNode_BufferReader::build(SignalGraphCompiler &c)
{
    std::cout << this << " Reader::build\n";

    if(m_out.connectedSinkCount() == 0)
        return;

    GPR64 regs[2];
    auto nregs = c.allocGPR(regs, 2);
    if(nregs == 0)
    {
        regs[0] = rax;
        c.PUSH(rax);
    }

    c.MOV(regs[0], ImmAddr(buffer() + c.frameCount()));
    if(nregs == 2)
    {
        c.MOV(regs[1], Base(regs[0]) + Index(c.mainLoopCounter(), 4));
        c.setStorage(m_out, regs + 1, 1);
    }
    else
    {
        c.MOV(regs[0], Base(regs[0]) + Index(c.mainLoopCounter(), 4));
        auto ptr = c.allocMemory(sizeof(float), sizeof(float));
        R64FX_DEBUG_ASSERT(ptr);
        c.MOV(Mem32(c.ptrMem<unsigned char*>(ptr)), GPR32(regs[0].gpr32()));
    }

    if(nregs == 0)
    {
        c.POP(rax);
    }
    else
    {
        c.freeGPR(regs, 1);
    }

    m_out.setSize(1);
    m_out.setScalarType(SDS::Float());
    m_out.setScalarSize(SDS::Single());
}


void SignalNode_BufferReader::cleanup(SignalGraphCompiler &c)
{
    R64FX_DEBUG_ASSERT(m_out.connectedSinkCount() == 0);
}


void SignalNode_BufferWriter::build(SignalGraphCompiler &c)
{
    if(m_in.connectedSource())
        c.ensureBuilt(m_in.connectedSource());

    std::cout << this << " Writer::build\n";

    auto &in = m_in.connectedSource()[0];
    R64FX_DEBUG_ASSERT((in.type() & SignalDataScalarType()) == SDS::Float());
    R64FX_DEBUG_ASSERT((in.type() & SignalDataScalarSize()) == SDS::Single());
    R64FX_DEBUG_ASSERT(in.size() == 1);

    GPR64 source_reg;
    if(in.isInRegisters())
    {
//         in.registerMap();
    }
    else
    {
        
    }

    GPR64 base_reg;
    auto nregs = c.allocGPR(&base_reg, 1);
}


void SignalNode_BufferWriter::cleanup(SignalGraphCompiler &c)
{
    R64FX_DEBUG_ASSERT(m_in.connectedSource() == nullptr);
}


}//namespace r64fx
