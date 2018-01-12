#include "SignalNode_BufferRW.hpp"

namespace r64fx{

void SignalNode_BufferReader::build()
{
    R64FX_DEBUG_ASSERT(buffer(0));

    float* buff[2] = {
        buffer(1) ? buffer(1) : buffer(0), buffer(0)
    };

    auto reg = allocRegisters<GPR64>(buffer(1) ? 2 : 1);

    MOV(reg[0], ImmAddr(buff[0] + frameCount()));
    MOV(reg[0].low32(), Base(reg[0]) + Index(rcx)*4);
    if(buffer(1))
    {
        MOV(reg[1], ImmAddr(buff[1] + frameCount()));
        MOV(reg[1].low32(), Base(reg[1]) + Index(rcx)*4);
        SHL(reg[0], Imm8(32));
        ADD(reg[0], reg[1]);
        freeRegisters(RegisterPack<GPR64>{reg[1]});

        m_out.setDataType(SignalDataType::Float32_Packed2);
    }
    else
    {
        m_out.setDataType(SignalDataType::Float32);
    }

    initStorage<float, GPR64>(m_out, reg[0]);
}


void SignalNode_BufferWriter::build()
{
    R64FX_DEBUG_ASSERT(buffer(0));

    GPR64 val0;

    auto source = m_in.connectedSource();
    if(source)
    {
        ensureBuilt(source);

        R64FX_DEBUG_ASSERT(source->size() == 1);
        R64FX_DEBUG_ASSERT(source->registerType() == register_type<GPR64>());
        if(source->hasRegisters())
        {
            source->lock();
            val0 = getStorageRegisters<GPR64>(*source);
        }
        else
        {
            R64FX_DEBUG_ASSERT(source->hasMemory());
            val0 = allocRegisters<GPR64>(1);
        }
    }
    else
    {
        val0 = allocRegisters<GPR64>(1);
        XOR(val0, val0);
    }

    GPR64 base = allocRegisters<GPR64>(1);
    MOV(base, ImmAddr(buffer(0) + frameCount()));
    MOV(Base(base) + Index(rcx)*4, val0.low32());
    if(buffer(1))
    {
        GPR64 val1 = allocRegisters<GPR64>(1);
        MOV(val1, val0);
        SHR(val1, Imm8(32));
        MOV(base, ImmAddr(buffer(1) + frameCount()));
        MOV(Base(base) + Index(rcx)*4, val1.low32());
        freeRegisters(RegisterPack<GPR64>{val1});
    }

    if(source)
    {
        source->unlock();
        sourceUsed(source);
    }
    else
    {
        freeRegisters(RegisterPack<GPR64>{val0});
    }
    freeRegisters(RegisterPack<GPR64>{base});
}

}//namespace r64fx
