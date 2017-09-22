#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SignalGraph;
class SignalGraphNode;
class SignalGraphProcessingContext;


class SignalSource{
    friend class SignalGraph;

    int m_connected_sinks = 0;
    int m_processed_sinks = 0;

public:
    SignalSource();

    ~SignalSource();
};


class SignalSink{
    friend class SignalGraphNode;

    SignalSource* m_connected_source = nullptr;

public:
    SignalSink();

    ~SignalSink();

    inline SignalSource* connectedSource() const { return m_connected_source; }
};


class SignalGraphNode : public LinkedList<SignalGraphNode>::Node{
    friend class SignalGraph;
    SignalGraph*  m_parent  = nullptr;
    unsigned long m_index   = 0;

public:
    SignalGraphNode();

    virtual ~SignalGraphNode();

    inline SignalGraph* parent() const { return m_parent; }

    virtual void process(SignalGraphProcessingContext* ctx) = 0;

    virtual void getSources(SignalSource* &sources, int &nsources);

    virtual void getSinks(SignalSink* &sinks, int &nsinks);

    void disconnectPorts();

    inline unsigned long index() const { return m_index; }

    virtual unsigned long enumerate(unsigned long first_index, unsigned long increment);
};


template<int SourceCount, int SinkCount> class SignalGraphNode_WithPorts : public SignalGraphNode{
    SignalSource  m_sources [SourceCount];
    SignalSink    m_sinks   [SinkCount];

public:
    virtual void getSources(SignalSource* &sources, int &nsources) override
    {
        sources = m_sources;
        nsources = SourceCount;
    }

    virtual void getSinks(SignalSink* &sinks, int &nsinks) override
    {
        sinks = m_sinks;
        nsinks = SinkCount;
    }

    inline SignalSource* source(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SourceCount);
#endif//R64FX_DEBUG
        return m_sources + i;
    }

    inline SignalSink* sink(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SinkCount);
#endif//R64FX_DEBUG
        return m_sinks + i;
    }

    inline int sourceCount() const { return SourceCount; }

    inline int sinkCount() const { return SinkCount; }
};


template<int SourceCount> class SignalGraphNode_WithSources : public SignalGraphNode{
    SignalSource m_sources[SourceCount];

public:
    virtual void getSources(SignalSource* &sources, int &nsources) override
    {
        sources = m_sources;
        nsources = SourceCount;
    }

    inline SignalSource* source(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SourceCount);
#endif//R64FX_DEBUG
        return m_sources + i;
    }

    inline int sourceCount() const { return SourceCount; }
};


template<int SinkCount> class SignalGraphNode_WithSinks : public SignalGraphNode{
    SignalSink m_sinks[SinkCount];

public:
    virtual void getSinks(SignalSink* &sinks, int &nsinks) override
    {
        sinks = m_sinks;
        nsinks = SinkCount;
    }

    inline SignalSink* sink(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SinkCount);
#endif//R64FX_DEBUG
        return m_sinks + i;
    }

    inline int sinkCount() const { return SinkCount; }
};


template<typename PortT> class NodePort{
    SignalGraphNode* m_node = nullptr;
    PortT*           m_port = nullptr;

public:
    NodePort(SignalGraphNode* node, PortT* port)
    : m_node(node), m_port(port) {}

    inline SignalGraphNode* node() const { return m_node; }

    inline PortT* port() const { return m_port; }
};

typedef NodePort<SignalSource>  NodeSource;
typedef NodePort<SignalSink>    NodeSink;


class SignalGraph : public SignalGraphNode{
    LinkedList<SignalGraphNode> m_nodes;
    unsigned long  m_node_count    = 0;
    unsigned long  m_sample_count  = 0;

public:
    virtual void process(SignalGraphProcessingContext* sgpctx) override final;

    void insertNode(SignalGraphNode* node);

    void removeNode(SignalGraphNode* node);

    void makeConnection(const NodeSource &node_source, const NodeSink &node_sink);

    void disconnectSink(const NodeSink &node_sink);

    void disconnectSource(const NodeSource &node_source);

    void disconnectNode(SignalGraphNode* node);

    virtual unsigned long enumerate(unsigned long first_index, unsigned long increment) override final;

    inline void setSampleCount(unsigned long sample_count) { m_sample_count = sample_count; }

    inline unsigned long sampleCount() const { return m_sample_count; }
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
