#ifndef R64FX_SIGNAL_NODE_HPP
#define R64FX_SIGNAL_NODE_HPP

#include "LinkedList.hpp"

namespace r64fx{

class SignalNodeClass;

class SignalNode : public LinkedList<SignalNode>::Node{
    friend class SignalNodeClass;

    int m_slot_count = 0;
    int m_slot_offset = 0;

    SignalNodeClass* m_parent_class;
    void setParentClass(SignalNodeClass* parent_class);

public:
    void setSlotCount(int count);

    int slotCount() const;

    void setSlotOffset(int offset);

    int slotOffset() const;
};


class SignalNodeClass : public LinkedList<SignalNodeClass>::Node{
    LinkedList<SignalNode> m_nodes;

public:
    SignalNode* newNode();

    void deleteNode(SignalNode* node);

    virtual void prepare() = 0;

    virtual void process() = 0;

    virtual void finish() = 0;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_HPP