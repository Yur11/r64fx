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
    virtual std::string name() = 0;

    virtual bool isOutput() = 0;
};


class NodeInput : public NodePort{

};


class NodeOutput : public NodePort{
    
};


class Node{
public:
    virtual void setSize(int size) = 0;

    virtual int size() = 0;
};


class NodeClass{
public:
    enum class Type{
        AudioInput,
        AudioOutput,
        MidiInput,
        MidiOutput,
        Oscillator,
        Player,
    };

    virtual std::string name() = 0;
};


class NodeClass_AudioInput
: public NodeClass{

};


class NodeClass_AudioOutput
: public NodeClass{

};


class NodeClass_MidiInput
: public NodeClass{

};


class NodeClass_MidiOutput
: public NodeClass{

};


class NodeClass_Oscillator
: public NodeClass{

};


class NodeClass_Player
: public NodeClass{

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

    virtual Node* newNode(NodeClass* node_class, int size) = 0;

    virtual void deleteNode(Node* node) = 0;

    static NodeGraph* newInstance(SoundDriver* sound_driver);

    static void deleteInstance(NodeGraph* graph);
};

}//namespace r64fx

#endif//R64FX_NODE_GRAPH_HPP