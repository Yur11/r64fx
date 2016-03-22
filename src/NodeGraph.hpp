#ifndef R64FX_NODE_GRAPH_HPP
#define R64FX_NODE_GRAPH_HPP

namespace r64fx{

class SoundDriver;

class NodeGraph{
    void* m = nullptr;

public:
    NodeGraph(SoundDriver* driver);

    ~NodeGraph();

    void enable();

    void disable();
};

}//namespace r64fx

#endif//R64FX_NODE_GRAPH_HPP