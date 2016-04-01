#include "SignalNode.hpp"
#include "SignalGraph.hpp"
#include <new>

namespace r64fx{

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


SignalNodeClass* SignalNode::parentClass() const
{
    return m_parent_class;
}


SignalNodeClass::SignalNodeClass(SignalGraph* parent_graph)
{
    setParentGraph(parent_graph);
}


int SignalNodeClass::size() const
{
    return m_size;
}


SignalNode* SignalNodeClass::newNode(int slot_count)
{
    SignalNode* node = new(std::nothrow) SignalNode;
    addNewNode(node, slot_count);
    return node;
}


void SignalNodeClass::deleteNode(SignalNode* node)
{
    if(node->parentClass() != this)
        return;

    m_nodes.remove(node);
    m_size -= node->slotCount();
    nodeRemoved(node);
    delete node;
}


void SignalNodeClass::forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg)
{

}


void SignalNodeClass::addNewNode(SignalNode* node, int slot_count)
{
    if(node)
    {
        int offset = 0;
        if(!m_nodes.isEmpty())
        {
            auto last_node = m_nodes.last();
            offset = last_node->slotOffset() + last_node->slotCount();
        }

        node->setParentClass(this);
        node->setSlotCount(slot_count);
        node->setSlotOffset(offset);
        m_nodes.append(node);
        m_size += slot_count;
        nodeAppended(node);
    }
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


void SignalNodeClass::nodeAppended(SignalNode* node)
{
    resizePorts();
}


void SignalNodeClass::nodeRemoved(SignalNode* node)
{
    resizePorts();
}


void SignalNodeClass::prepare()
{

}


void SignalNodeClass::process(int sample)
{

}


void SignalNodeClass::finish()
{

}


void SignalNodeClass::setNodeData(SignalNode* node, void* data)
{
    node->data = data;
}


void* SignalNodeClass::getNodeData(SignalNode* node)
{
    return node->data;
}


void SignalNodeClass::resizePorts()
{
    forEachPort([](SignalPort* port, void* arg){
        auto node_class = (SignalNodeClass*)arg;
        port->resize(node_class->size());
    }, this);
}

}//namespace r64fx