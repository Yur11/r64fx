#include "SignalNode_BufferRW.hpp"

namespace r64fx{

void SignalNode_BufferReader::build()
{
    GPR64 reg = allocRegisters<GPR64>(1);
    MOV(reg, ImmAddr(addr(buffer()) + frameCount()));
    MOV(reg.low32(), Base(reg) + Index(rcx)*4);
//     XOR(reg, reg);
//     SHL(reg);
    OR(reg, Imm32(0x555555));

    initStorage<float, GPR64>(m_out, reg);
}


void SignalNode_BufferWriter::build()
{
    GPR64 srcval;

    auto source = m_in.connectedSource();
    if(source)
    {
        ensureBuilt(source);

        R64FX_DEBUG_ASSERT(source->size() == 1);
        R64FX_DEBUG_ASSERT(source->registerType() == register_type<GPR64>());
        if(source->hasRegisters())
        {
            source->lock();
            srcval = getStorageRegisters<GPR64>(*source);
        }
        else
        {
            R64FX_DEBUG_ASSERT(source->hasMemory());
            srcval = allocRegisters<GPR64>(1);
        }
    }
    else
    {
        srcval = allocRegisters<GPR64>(1);
        XOR(srcval, srcval);
    }

    GPR64 base = allocRegisters<GPR64>(1);
    MOV(base, ImmAddr(addr(buffer()) + frameCount()));
    MOV(Base(base) + Index(rcx)*4, srcval.low32());

    if(source)
    {
        source->unlock();
        sourceUsed(source);
    }
    else
    {
        freeRegisters(RegisterPack<GPR64>{srcval});
    }
    freeRegisters(RegisterPack<GPR64>{base});
}

}//namespace r64fx
