#include "SignalNode_BufferRW.hpp"
#include "SignalNodeFlags.hpp"

namespace r64fx{

void SignalNode_BufferReader::build()
{
    auto regs = allocRegisters<GPR64>(1);
    R64FX_DEBUG_ASSERT(regs.size() == 1);
    MOV(regs[0], ImmAddr(buffer() + frameCount()));
    MOV(regs[0], Base(regs[0]) + Index(rcx)*4);
    initStorage<float, GPR64>(m_out, regs[0]);
}


void SignalNode_BufferWriter::build()
{
    GPR64 value;

    auto source = m_in.connectedSource();
    if(source)
    {
        ensureBuilt(source);

        R64FX_DEBUG_ASSERT(source->size() == 1);
        std::cout << "reg_type: " << int(source->registerType()) << "\n";
        R64FX_DEBUG_ASSERT(source->registerType() == register_type<GPR64>());
        if(source->hasRegisters())
        {
            source->lock();
        }
        else
        {
            R64FX_DEBUG_ASSERT(source->hasMemory());
        }
        value = getStorageRegisters<GPR64>(*source)[0];
    }
    else
    {
        auto pack = allocRegisters<GPR64>(1);
        R64FX_DEBUG_ASSERT(pack.size() == 0);
        value = pack[0];
        XOR(value, value);
    }

    auto base = allocRegisters<GPR64>(1);
    R64FX_DEBUG_ASSERT(base.size() == 1);
    MOV(base[0], ImmAddr(buffer() + frameCount()));
    MOV(Base(base[0]) + Index(rcx)*4, value);

    if(source)
    {
        source->unlock();
    }
}

}//namespace r64fx
