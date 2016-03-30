#include "SignalNode.hpp"
#include "SignalGraph.hpp"
#include <new>

namespace r64fx{

LinkedList<SignalNode> g_spare_nodes;


void SignalNode::setParentClass(SignalNodeClass* parent_class)
{
    m_parent_class = parent_class;
}


void SignalNode::setSlotCount(int slot_count)
{
    m_slot_count = slot_count;
}


int SignalNode::slotCount() const
{
    return m_slot_count;
}


void SignalNode::setSlotOffset(int offset)
{
    m_slot_offset = offset;
}


int SignalNode::slotOffset() const
{
    return m_slot_offset;
}


SignalNodeClass::SignalNodeClass(SignalGraph* parent_graph)
{
    setParentGraph(parent_graph);
}


SignalNode* SignalNodeClass::newNode(int slot_count)
{
    SignalNode* node = nullptr;
    if(g_spare_nodes.isEmpty())
    {
        node = new(std::nothrow) SignalNode;
    }
    else
    {
        node = g_spare_nodes.last();
        g_spare_nodes.remove(node);
    }

    if(node)
    {
        int offset = 0;
        if(!g_spare_nodes.isEmpty())
        {
            auto last_node = g_spare_nodes.last();
            offset = last_node->slotOffset() + last_node->slotCount();
        }

        node->setParentClass(this);
        node->setSlotCount(slot_count);
        node->setSlotOffset(offset);
        m_nodes.append(node);
        nodeAppended(node);
    }

    return node;
}


void SignalNodeClass::deleteNode(SignalNode* node)
{

}


int SignalNodeClass::totalSlotCount() const
{
    int count = 0;
    for(auto node : m_nodes)
    {
        count += node->slotCount();
    }
    return count;
}


void SignalNodeClass::setParentGraph(SignalGraph* parent_graph)
{
    m_parent_graph = parent_graph;
    parent_graph->m_node_classes.append(this);
}


SignalGraph* SignalNodeClass::parentGraph() const
{
    return m_parent_graph;
}


int SignalNodeClass::bufferSize() const
{
    return m_parent_graph->bufferSize();
}


float SignalNodeClass::sampleRate() const
{
    return m_parent_graph->sampleRate();
}


float SignalNodeClass::sampleRateReciprocal() const
{
    return m_parent_graph->sampleRateReciprocal();
}


void SignalNodeClass::setNodeData(SignalNode* node, void* data, int i)
{
    node->data[i] = data;
}


void* SignalNodeClass::getNodeData(SignalNode* node, int i)
{
    return node->data[i];
}

}//namespace r64fx