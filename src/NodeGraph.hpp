#ifndef R64FX_NODE_GRAPH_HPP
#define R64FX_NODE_GRAPH_HPP

#include <string>
#include "LinkedList.hpp"

namespace r64fx{

class SoundDriver;


class NodePort{
protected:
    virtual ~NodePort() {}

public:
    enum class Type{
        Signal,
        Notes
    };

    virtual Type type() = 0;

    virtual std::string name() = 0;

    virtual bool isOutput() = 0;
};


class NodeInput : public NodePort{
public:
    virtual bool isOutput() = 0;
};


class NodeOutput : public NodePort{
public:
    virtual bool isOutput() = 0;
};


class Node{
public:
    virtual void setSize(int size) = 0;

    virtual int size() = 0;
};


class NodeClass{
public:
    virtual std::string name() = 0;

    virtual Node* newNode() = 0;
};


class NodeClass_AudioInput : public NodeClass{

};


class NodeClass_AudioOutput : public NodeClass{

};


class NodeClass_MidiIO : public NodeClass{

};


class NodeClass_Oscillator : public NodeClass{

};


class NodeClass_Player : public NodeClass{

};


class NodeLink{
public:
    enum class Type{
        Write,
        Mix,
        Modulate
    };

    virtual Type type() = 0;
};


class NodeGraph{
    void* m = nullptr;

protected:
    NodeGraph(){}

    virtual ~NodeGraph() {};

public:
    virtual void enable() = 0;

    virtual void disable() = 0;

//     virtual IteratorPair<NodeClass*> nodeClasses() = 0;
//
//     virtual Node* newNode(NodeClass* node_class) = 0;
//
//     virtual NodeLink* newNodeLink(
//         Node* dst, NodeInput* dst_port,
//         Node* src, NodePort* src_port,
//         NodeLink::Type type = NodeLink::Type::Write,
//         int* dst_slots = nullptr, int num_dst_slots = 0,
//         int* src_slots = nullptr, int num_src_slots = 0
//     ) = 0;
//
//     virtual IteratorPair<NodeLink> nodeLinks() = 0;

    static NodeGraph* newInstance(SoundDriver* sound_driver);

    static void deleteInstance(NodeGraph* graph);
};

}//namespace r64fx

#endif//R64FX_NODE_GRAPH_HPP