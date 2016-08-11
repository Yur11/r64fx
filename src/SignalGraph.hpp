#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"

namespace r64fx{
    
class SignalGraph;

/* Any item that can be processed inside a SignalGraph. */
class SignalGraphProcessable : public LinkedList<SignalGraphProcessable>::Node{
    SignalGraph* m_graph = nullptr;
    
public:    
    virtual ~SignalGraphProcessable();
    
    inline void setGraph(SignalGraph* graph) { m_graph = graph; }
    
    inline SignalGraph* graph() const { return m_graph; }
    
    /* Called at the beginning of a cycle. */
    virtual void prepare();
    
    /* Called for each sample during a cycle. */
    virtual void processSample(int i);
    
    /* Called at the end of a cycle. */
    virtual void finish();
};


class SignalPort{
    float* m_addr = nullptr;
    
public:
    SignalPort(float* addr) : m_addr(addr) {}
    
    SignalPort() {}
    
    inline float* addr() const { return m_addr; }
    
    inline float &operator[](int i) { return m_addr[i]; }
    
    inline float &operator*() { return m_addr[0]; }
};


class SignalSource : public SignalPort{
public:
    using SignalPort::SignalPort;
};


class SignalSink : public SignalPort{
public:
    using SignalPort::SignalPort;    
};


class SignalConnection : public SignalGraphProcessable{
    SignalSource* m_source;
    SignalSink*   m_sink;
    
public:
    SignalConnection(SignalSource* source, SignalSink* sink)
    : m_source(source)
    , m_sink(sink)
    {}
    
    inline SignalSource* source() const { return m_source; }
    
    inline SignalSink* sink() const { return m_sink; }
    
    virtual void processSample(int i);
};


class SignalNode : public SignalGraphProcessable{
    
};


class SignalNodeClass : public SignalGraphProcessable{
    
};


class SignalGraph : public SignalGraphProcessable{
    LinkedList<SignalGraphProcessable> m_single_nodes;
    LinkedList<SignalGraphProcessable> m_node_classes;
    LinkedList<SignalGraphProcessable> m_connections;
    LinkedList<SignalGraphProcessable> m_subgraphs;
    
public:
    void addNode(SignalNode* node);
    
    void removeNode(SignalNode* node);
    
    void addConnection(SignalConnection* connection);
    
    virtual void prepare();
    
    virtual void processSample(int i);
    
    virtual void finish();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP