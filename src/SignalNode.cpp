#include "SignalNode.hpp"

namespace r64fx{

void SignalNode::setParentClass(SignalNodeClass* parent_class)
{
    m_parent_class = parent_class;
}


void SignalNode::setSlotCount(int count)
{
    m_slot_count = count;
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


SignalNode* SignalNodeClass::newNode()
{
    return nullptr;
}


void SignalNodeClass::deleteNode(SignalNode* node)
{

}

}//namespace r64fx