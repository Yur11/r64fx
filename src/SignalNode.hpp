#ifndef R64FX_SIGNAL_NODE_HPP
#define R64FX_SIGNAL_NODE_HPP

#include "LinkedList.hpp"
#include "SignalPort.hpp"

namespace r64fx{

class SignalGraph;
class SignalNodeClass;
class SoundDriver;

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
    int m_size = 0;

protected:
    LinkedList<SignalNode> m_nodes;

public:
    SignalNodeClass(SignalGraph* parent_graph);

    int size() const;

    SignalNode* newNode(int slot_count = 1);

    void deleteNode(SignalNode* node);

    virtual void forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg) = 0;

protected:
    void addNewNode(SignalNode* node, int slot_count = 1);

    void removeNode(SignalNode* node);

    void setParentGraph(SignalGraph* parent_graph);

    SignalGraph* parentGraph() const;

    SoundDriver* soundDriver() const;

    int bufferSize() const;

    float sampleRate() const;

    float sampleRateReciprocal() const;

    virtual void nodeAppended(SignalNode* node);

    virtual void nodeRemoved(SignalNode* node);

    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

    static void setNodeData(SignalNode* node, void* data);

    static void* getNodeData(SignalNode* node);

    void resizePorts();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_HPP