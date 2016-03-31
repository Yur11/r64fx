#ifndef R64FX_SIGNAL_NODE_HPP
#define R64FX_SIGNAL_NODE_HPP

#include "LinkedList.hpp"
#include "SignalPort.hpp"

namespace r64fx{

class SignalGraph;
class SignalNodeClass;

class SignalNode : public LinkedList<SignalNode>::Node{
    friend class SignalNodeClass;

    int m_slot_count = 0;
    int m_slot_offset = 0;

    SignalNodeClass* m_parent_class;
    void setParentClass(SignalNodeClass* parent_class);

    void* data = nullptr;

public:
    void setSlotCount(int slot_count);

    int slotCount() const;

    void setSlotOffset(int offset);

    int slotOffset() const;

    SignalNodeClass* parentClass() const;
};


class SignalNodeClass : public LinkedList<SignalNodeClass>::Node{
    friend class SignalGraph;
    SignalGraph* m_parent_graph = nullptr;

protected:
    LinkedList<SignalNode> m_nodes;

public:
    SignalNodeClass(SignalGraph* parent_graph);

    virtual SignalNode* newNode(int slot_count = 1);

    virtual void deleteNode(SignalNode* node);

    int totalSlotCount() const;

    virtual void forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg);

protected:
    void setParentGraph(SignalGraph* parent_graph);

    SignalGraph* parentGraph() const;

    int bufferSize() const;

    float sampleRate() const;

    float sampleRateReciprocal() const;

    virtual void prepare() = 0;

    virtual void process(int sample) = 0;

    virtual void finish() = 0;

    virtual void nodeAppended(SignalNode* node) = 0;

    virtual void nodeRemoved(SignalNode* node) = 0;

    static void setNodeData(SignalNode* node, void* data);

    static void* getNodeData(SignalNode* node);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_HPP