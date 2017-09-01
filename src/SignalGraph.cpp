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


SignalPort::SignalPort(SignalNode* parent)
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


SignalNode* SignalPort::parentNode() const
{
    long thisnum = (long)this;
    long diff = m_bits;
    return (SignalNode*)(thisnum - diff);
}


bool SignalPort::isSource() const
{
    return m_bits & R64FX_SIGNAL_PORT_IS_SOURCE;
}


SignalSource::SignalSource(SignalNode* parent)
: SignalPort(parent)
{
    m_bits |= R64FX_SIGNAL_PORT_IS_SOURCE;
}


SignalSource::~SignalSource()
{

}


SignalSink::SignalSink(SignalNode* parent)
: SignalPort(parent)
{

}


SignalSink::~SignalSink()
{

}


SignalNode::SignalNode(long size)
: m_size(size)
{

}


SignalNode::~SignalNode()
{

}


SignalEdge::SignalEdge(SignalSource* source, SignalSink* sink)
{

}


SignalEdge::~SignalEdge()
{

}

}//namespace r64fx
