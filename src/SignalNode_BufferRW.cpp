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
        c.setStorage(m_out, ptr);
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
}


void SignalNode_BufferReader::cleanup(SignalGraphCompiler &c)
{
    R64FX_DEBUG_ASSERT(m_out.connectedSinkCount() == 0);
    c.freeStorage(m_out);
}


void SignalNode_BufferWriter::build(SignalGraphCompiler &c)
{
    if(m_in.connectedSource())
        c.ensureBuilt(m_in.connectedSource());
    else
        return;

    std::cout << this << " Writer::build\n";

    auto &source = m_in.connectedSource()[0];
    R64FX_DEBUG_ASSERT(source.isSingle());
    R64FX_DEBUG_ASSERT(source.size() == 1);

    GPR64 source_reg;
    bool restore_source_reg = false;
    if(source.isGPR())
    {
        unsigned int nregs = 0;
        c.getStorage(source, &source_reg, &nregs);
        R64FX_DEBUG_ASSERT(nregs == 1);
    }
    else if(source.isMemory())
    {
        restore_source_reg = (c.allocGPR(&source_reg, 1) == 0);
        if(restore_source_reg)
            c.PUSH(source_reg);
        c.MOV(source_reg.gpr32(), Mem32(c.ptrMem<unsigned char*>(c.getPtr(source))));
    }

    GPR64 base_reg;
    bool restore_base_reg = (c.allocGPR(&base_reg, 1) == 0);
    if(restore_base_reg)
    {
        base_reg = rax;
        if(base_reg == source_reg)
            base_reg = rdx;
        c.PUSH(base_reg);
    }

    c.MOV(base_reg, ImmAddr(buffer() + c.frameCount()));
    c.MOV(Base(base_reg) + Index(c.mainLoopCounter(), 4), source_reg);

    if(restore_base_reg)
        c.POP(base_reg);
    else
        c.freeGPR(&base_reg, 1);

    if(restore_source_reg)
        c.POP(source_reg);
    else if(source.isMemory() || source.processedSinkCount() == 0)
        c.freeGPR(&source_reg, 1);
}


void SignalNode_BufferWriter::cleanup(SignalGraphCompiler &c)
{
    R64FX_DEBUG_ASSERT(m_in.connectedSource() == nullptr);
}


}//namespace r64fx
