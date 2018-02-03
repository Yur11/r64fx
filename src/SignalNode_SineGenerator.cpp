#include "SignalNode_SineGenerator.hpp"
#include "jit_algorithms.hpp"

namespace r64fx{

JitAlgorithmConstants g_constants __attribute__((aligned(16)));

SignalNode_SineGenerator::SignalNode_SineGenerator(SignalGraph* sg)
: SignalNode(sg), m_out(this)
{
    m_data = allocMemory<float>(8);
    auto buffers = new(addr(m_data)) SineOscBuffers;
    for(int i=0; i<4; i++)
        buffers->v1.increment[i] = (440.0f / 48000.0f) * 0xFFFFFFFFU;
}

SignalNode_SineGenerator::~SignalNode_SineGenerator()
{
    freeMemory(m_data);
}

void SignalNode_SineGenerator::build()
{
    if(m_out.connectedSinkCount() == 0)
        return;

    auto xmms = allocRegisters<Xmm>(5);
    R64FX_DEBUG_ASSERT(xmms.size() == 5);

    auto gprs = allocRegisters<GPR64>(2);
    R64FX_DEBUG_ASSERT(gprs.size() == 2);

    MOV(gprs[0], ImmAddr(&g_constants));
    MOV(gprs[1], ImmAddr(addr(m_data)));

    gen_sine_osc(this, gprs[0], gprs[1], xmms[0], xmms[1], xmms[2], xmms[3], xmms[4]);

    freeRegisters(gprs);
    freeRegisters(RegisterPack<Xmm>{xmms[1], xmms[2], xmms[3], xmms[4]});

    m_out.setDataType(SignalDataType::Float32);
    initStorage<float, Xmm>(m_out, xmms[0]);
}


SignalNode_ScalarExtractor::SignalNode_ScalarExtractor(SignalGraph* sg)
: SignalNode(sg), m_out(this)
{
    
}


SignalNode_ScalarExtractor::~SignalNode_ScalarExtractor()
{
    
}


void SignalNode_ScalarExtractor::build()
{
    if(m_out.connectedSinkCount() == 0)
        return;

    auto result = allocRegisters<GPR64>(1);
    R64FX_DEBUG_ASSERT(result.size() == 1);

    auto source = m_in.connectedSource();
    ensureBuilt(source);
    if(source)
    {
        R64FX_DEBUG_ASSERT(source->hasRegisters());
        auto source_regs = getStorageRegisters<Xmm>(*source);
        R64FX_DEBUG_ASSERT(source_regs.size() == 1);
        MOVD(result[0].low32(), source_regs[0]);
    }
    else
    {
        XOR(result[0], result[0]);
    }
    sourceUsed(source);

    m_out.setDataType(source->dataType());
    initStorage<float, GPR64>(m_out, result);
}

}//namespace
