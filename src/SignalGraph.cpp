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


unsigned long SignalGraphNode::enumerate(unsigned long first_index, unsigned long increment)
{
    m_index = first_index;
    return first_index + increment;
}


void SignalGraph::process(SignalGraphProcessingContext* sgpctx)
{
    auto a = sgpctx->assembler();
    a->push(rbx);
    a->mov(rbx, Imm64(0));
    auto loop = a->ip();
    for(auto node : m_nodes)
    {
        node->process(sgpctx);
    }
    a->add(rbx, Imm32(1));
    a->cmp(rbx, Imm32(sampleCount()));
    a->jne(loop);
    a->pop(rbx);
}


void SignalGraph::insertNode(SignalGraphNode* node)
{
#ifdef R64FX_DEBUG
    assert(node);
    assert(node->m_parent == nullptr);
#endif//R64FX_DEBUG

    SignalSource*  sources       = nullptr;
    int            source_count  = 0;
    node->getSources(sources, source_count);

    SignalSink*    sinks         = nullptr;
    int            sink_count    = 0;
    node->getSinks(sinks, sink_count);

    if(sink_count)
    {
        if(source_count)
        {
            
        }
        else
        {
            m_nodes.append(node);
        }
    }
    else
    {
        if(source_count)
        {
            m_nodes.preppend(node);
        }
        else
        {
            m_nodes.append(node);
        }
    }

    node->m_parent = this;;
}


void SignalGraph::removeNode(SignalGraphNode* node)
{
    m_nodes.remove(node);
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


unsigned long SignalGraph::enumerate(unsigned long first_index, unsigned long increment)
{
    m_index = first_index;
    unsigned long index = m_index + increment;
    for(auto node : m_nodes)
    {
        index = node->enumerate(index, increment);
    }
    return index;
}

}//namespace r64fx
