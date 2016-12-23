#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{


void SignalEdge::setParent(SignalNode* node)
{
    if(m_parent)
        m_parent->m_edges.remove(this);
    m_parent = node;
    if(m_parent)
        node->m_edges.append(this);
}


SignalNode* SignalEdge::parent() const
{
    return m_parent;
}


void SignalNode::setParent(SignalNode* node)
{
    if(m_parent)
        m_parent->m_nodes.remove(this);
    m_parent = node;
    if(m_parent)
        node->m_nodes.append(this);
}


SignalNode* SignalNode::parent() const
{
    return m_parent;
}


IteratorPair<LinkedList<SignalNode>::Iterator> SignalNode::nodes() const
{
    return {m_nodes.begin(), m_nodes.end()};
}


IteratorPair<LinkedList<SignalEdge>::Iterator> SignalNode::edges() const
{
    return {m_edges.begin(), m_edges.end()};
}

}//namespace r64fx
