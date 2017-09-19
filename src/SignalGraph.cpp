#include "SignalGraph.hpp"
#include "SignalGraphProcessingContext.hpp"
#include "jit.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

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


SignalGraphNode::SignalGraphNode()
{

}


SignalGraphNode::~SignalGraphNode()
{

}


void SignalGraphNode::getSources(SignalSource*&, int &nsources)
{
    nsources = 0;
}


void SignalGraphNode::getSinks(SignalSink*&, int &nsinks)
{
    nsinks = 0;
}


void SignalGraph::process(SignalGraphProcessingContext* sgpctx)
{
    auto a = sgpctx->assembler();
    a->push(rbx);
    a->mov(rbx, Imm64U(0));
    auto loop = a->codeEnd();
    for(auto node : m_nodes)
    {
        node->process(sgpctx);
    }
    a->add(rbx, Imm32U(1));
    a->cmp(rbx, Imm32U(sampleCount()));
    a->jne(loop);
    a->pop(rbx);
}


void SignalGraph::insertNode(SignalGraphNode* node)
{

}


void SignalGraph::removeNode(SignalGraphNode* node)
{
    
}


void SignalGraph::makeConnection(const NodeSource &node_source, const NodeSink &node_sink)
{
    
}


void SignalGraph::disconnectSink(const NodeSink &node_sink)
{
    
}


void SignalGraph::disconnectSource(const NodeSource &node_source)
{
    
}


void SignalGraph::disconnectNode(SignalGraphNode* node)
{
    
}

}//namespace r64fx
