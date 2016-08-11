#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"

namespace r64fx{
    
class SignalGraph;

/* Any item that can be processed inside a SignalGraph. */
class SignalGraphProcessable : public LinkedList<SignalGraphProcessable>::Node{
    friend class SignalGraph;
    
    SignalGraph* m_graph = nullptr;

    inline void setGraph(SignalGraph* graph) { m_graph = graph; }
    
public:    
    virtual ~SignalGraphProcessable();
    
    void removeFromGraph();
    
    inline SignalGraph* graph() const { return m_graph; }
    
private:
    /* Called after this node has been added to a graph. */
    virtual void addedToGraph(SignalGraph* graph);
    
    /* Called before this node is removed from its graph. */
    virtual void aboutToBeRemovedFromGraph(SignalGraph* graph);
    
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
    
    inline void setAddr(float* addr) { m_addr = addr; }
    
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
    SignalSource* m_source = nullptr;
    SignalSink*   m_sink = nullptr;
    
public:
    SignalConnection(SignalSource* source, SignalSink* sink);
    
    void update(SignalSource* source, SignalSink* sink);
    
    void setSource(SignalSource* source);
    
    SignalSource* source() const;
    
    void setSink(SignalSink* sink);
    
    SignalSink* sink() const;
    
    virtual void processSample(int i);
};


class SignalNode : public SignalGraphProcessable{
    
};


class SignalNodeClass : public SignalGraphProcessable{
    
};


class SignalGraph : public SignalGraphProcessable{
    LinkedList<SignalGraphProcessable> m_items;
    
public:
    void addItem(SignalGraphProcessable* item);
    
    void removeItem(SignalGraphProcessable* item);
    
    void clear();
    
    void replaceSource(SignalSource* old_source, SignalSource* new_source);
    
    void replaceSink(SignalSink* old_sink, SignalSink* new_sink);
    
    virtual void prepare();
    
    virtual void processSample(int i);
    
    virtual void finish();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP