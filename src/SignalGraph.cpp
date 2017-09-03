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
#endif//R64FX_DEBUG
    m_bits = diff;
    m_ptr = new float[parent->size()];
}


SignalPort::~SignalPort()
{

}


SignalGraphNode* SignalPort::parentNode() const
{
    long thisnum = (long)this;
    long diff = m_bits;
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


SignalGraphNode::SignalGraphNode(long size)
: m_size(size)
{

}


SignalGraphNode::~SignalGraphNode()
{

}


SignalGraphEdge::SignalGraphEdge(SignalSource* source, SignalSink* sink)
{

}


SignalGraphEdge::~SignalGraphEdge()
{

}

}//namespace r64fx
