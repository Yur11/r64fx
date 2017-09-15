#include "SignalGraph.hpp"

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


void SignalSink::setConnectedSource(SignalSource* source)
{
    if(m_connected_source)
    {
        m_connected_source->m_connected_sinks--;
    }

    m_connected_source = source;
    m_connected_source->m_connected_sinks++;
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
    for(auto node : m_nodes)
    {
        node->process(sgpctx);
    }
}


void SignalGraph::insertNode(SignalGraphNode* node, SignalGraphNode* after, SignalGraphNode* before)
{
    if(m_nodes.isEmpty())
    {
        m_nodes.append(node);
        return;
    }
}


void SignalGraph::removeNode(SignalGraphNode* node)
{
    m_nodes.remove(node);
}


void SignalGraph::sort()
{
    
}

}//namespace r64fx
