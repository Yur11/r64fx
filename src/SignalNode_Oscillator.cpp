#include "SignalNode_Oscillator.hpp"

namespace r64fx{

SignalNode_OscClock::SignalNode_OscClock(SignalGraph* sg)
: SignalNode(sg)
, m_out(this)
{
    
}


void SignalNode_OscClock::build()
{
    std::cout << "SignalNode_OscClock\n";
/*
    if(!m_clock)
    {
        m_clock = c.allocMemory(sizeof(float) * 4, sizeof(float) * 4);
        auto buff = c.ptrMem<float*>(m_clock);
        for(int i=0; i<4; i++) buff[i] = 0.0f;
    }

    GPR64 gpr;
    auto ngprs = c.allocGPR(&gpr, 1);
    if(ngprs == 0)
    {
        c.PUSH(rax);
    }

    Xmm xmm;
    auto nxmms = c.allocXmm(&xmm, 1);
    R64FX_DEBUG_ASSERT(nxmms == 1);

    c.MOV(gpr, Imm32(m_delta));
    c.MOVD(xmm, gpr.gpr32());
    c.PSHUFD(xmm, xmm, Shuf(0, 0, 0, 0));
    c.PADDD(xmm, Mem128(c.ptrMem<float*>(m_clock)));
    c.MOVAPS(Mem128(c.ptrMem<float*>(m_clock)), xmm);
    c.CVTDQ2PS(xmm, xmm);
    c.MOVD(gpr.gpr32(), xmm);

    c.freeXmm(&xmm, 1);

    if(ngprs == 0)
    {
        c.POP(rax);
    }
    else
    {
        c.setStorage(m_out, &gpr, 1);
    }

    m_out.setSize(1);*/
}

}//namespace r64fx
