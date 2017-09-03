#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

SignalGraphElement::SignalGraphElement()
{

}


SignalGraphElement::~SignalGraphElement()
{

}


#define R64FX_SIGNAL_PORT_IS_SOURCE           0x8000000000000000UL
#define R64FX_SIGNAL_PORT_IS_CONNECTED        0x4000000000000000UL
#define R64FX_SIGNAL_PORT_PARENT_OFFSET_MASK  0x3FFFFFFFFFFFFFFFUL


SignalPort::SignalPort(SignalGraphNode* parent)
{
    long thisnum    = (long)this;
    long parentnum  = (long)parent;
    long diff = thisnum - parentnum;
#ifdef R64FX_DEBUG
    assert(parentnum > 0);
    assert(diff >= 0);
    assert(diff <= long(R64FX_SIGNAL_PORT_PARENT_OFFSET_MASK));
#endif//R64FX_DEBUG
    m_bits |= (diff & R64FX_SIGNAL_PORT_PARENT_OFFSET_MASK);
}


SignalPort::~SignalPort()
{

}


SignalGraphNode* SignalPort::parentNode() const
{
    long thisnum = (long)this;
    long diff = (m_bits & R64FX_SIGNAL_PORT_PARENT_OFFSET_MASK);
    return (SignalGraphNode*)(thisnum - diff);
}


bool SignalPort::isSource() const
{
    return m_bits & R64FX_SIGNAL_PORT_IS_SOURCE;
}


SignalSource::SignalSource(SignalGraphNode* parent)
: SignalPort(parent)
{
    m_bits |= R64FX_SIGNAL_PORT_IS_SOURCE;
}


SignalSource::~SignalSource()
{

}


SignalSink::SignalSink(SignalGraphNode* parent)
: SignalPort(parent)
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


void SignalGraphNode::resize(int new_size)
{
    if(m_size == new_size)
        return;

    auto pc = portCount();
    if(pc <= 0)
        return;

    struct Args{
        float* old_ptr  = nullptr;
        float* new_ptr   = nullptr;
    } args;

    if(new_size > 0)
    {
        args.new_ptr = new float[pc * new_size];
    }

    forEachPort([](SignalGraphNode* node, SignalPort* port, void* arg){
        auto args = (Args*) arg;

        if(!args->old_ptr)
            args->old_ptr = port->m_ptr;

        port->m_ptr = args->new_ptr;
        if(args->new_ptr)
            args->new_ptr++;

        return true;
    }, &args);

    if(args.old_ptr)
        delete args.old_ptr;
}


SignalGraphEdge::SignalGraphEdge(SignalSource* source, short source_offset, SignalSink* sink, short sink_offset, int size)
{
    auto source_node = source->parentNode();
    auto sink_node = sink->parentNode();
#ifdef R64FX_DEBUG
    assert((source_offset + size) < source_node->size());
    assert((sink_offset + size) < sink_node->size());
#endif//R64FX_DEBUG
    m_source         = source;
    m_sink           = sink;
    m_source_offset  = source_offset;
    m_sink_offset    = sink_offset;
    m_size           = size;
}


SignalGraphEdge::~SignalGraphEdge()
{

}


void SignalGraphEdge::routine(int)
{
    float* source_ptr = m_source->m_ptr + m_source_offset;
    float* sink_ptr = m_sink->m_ptr + m_sink_offset;
    for(int n=0; n<m_size; n++)
    {
        sink_ptr[n] = source_ptr[n];
    }
}


void SignalGraph::add(SignalGraphNode* node)
{
    m_elements.append(node);
}


void SignalGraph::add(SignalGraphEdge* edge)
{
    m_elements.append(edge);
}


void SignalGraph::remove(SignalGraphNode* node)
{
    m_elements.remove(node);
}


void SignalGraph::remove(SignalGraphEdge* edge)
{
    m_elements.remove(edge);
}


void SignalGraph::run(int nsamples)
{
    for(int i=0; i<nsamples; i++)
    {
        for(auto element : m_elements)
        {
            element->routine(i);
        }
    }
}

}//namespace r64fx
