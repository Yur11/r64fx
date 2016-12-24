#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

SignalGraphElement::~SignalGraphElement()
{
    
}


void SignalGraphElement::setParent(SignalNode* node)
{
    if(m_parent)
        m_parent->m_subgraph.remove(this);
    m_parent = node;
    if(m_parent)
        node->m_subgraph.append(this);
}


SignalNode* SignalGraphElement::parent() const
{
    return m_parent;
}


SignalPort::SignalPort(SignalNode* parent)
: m_addr(new float)
, m_parent(parent)
{
    
}


SignalPort::~SignalPort()
{
    delete m_addr;
}


SignalNode::SignalNode()
{
    
}


SignalNode::~SignalNode()
{
    
}


void SignalNode::prologue()
{
    
}


void SignalNode::routine(int i)
{
    
}


void SignalNode::epilogue()
{
    
}


SignalEdge::SignalEdge(SignalSource* source, SignalSink* sink, SignalNode* parent)
{
    
}


SignalEdge::~SignalEdge()
{
    
}


void SignalEdge::routine(int i)
{
    (void)i;
}

}//namespace r64fx
