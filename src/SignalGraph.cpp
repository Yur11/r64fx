#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

template<typename RegT, int MaxRegCount> inline void get_registers(long bits, RegT* regs, int* nregs)
{
    bits &= (MaxRegCount - 1);
    int n = 0;
    for(int i=0; bits && i<16; i++)
    {
        long b = (1 << i);
        if(bits & b)
        {
            regs[n++] = i;
            bits &= ~b;
        }
    }
    *nregs = n;
}

void SignalDataStorage_Xmm::getRegisters(Xmm* regs, int* nregs)
{
    get_registers<Xmm, 16>(u.l, regs, nregs);
}


SignalSource::SignalSource()
{

}


SignalSource::~SignalSource()
{

}


SignalSink::SignalSink()
{

}


SignalSink::~SignalSink()
{

}


SignalNode::SignalNode()
{

}


SignalNode::~SignalNode()
{
}


void SignalNode::getSources(SignalSource*&, int &nsources)
{
    nsources = 0;
}


void SignalNode::getSinks(SignalSink*&, int &nsinks)
{
    nsinks = 0;
}


SignalGraph::SignalGraph()
{

}


SignalGraph::~SignalGraph()
{

}


void SignalGraph::addNode(SignalNode* node)
{

}


void SignalGraph::removeNode(SignalNode* node)
{

}


void SignalGraph::build(SignalGraphProcessor &sgp)
{
    auto &as = sgp.assembler();

    as.push(rax);
    as.mov(rax, Imm32(-frameCount()));
    JumpLabel loop = as.ip();
    for(auto node : m_nodes)
        node->build(sgp);
    as.add(rax, Imm32(1));
    as.jnz(loop);
    as.pop(rax);
}


#define R64FX_SIGNAL_GRAPH_PROCESSOR_GPR_BITS 0xF
#define R64FX_GPR_BITS


void SignalGraphProcessor::build(SignalGraph &sg)
{
    m_assembler.rewind();
    sg.build(*this);
    m_assembler.ret();
}

}//namespace r64fx
