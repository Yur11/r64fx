#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SignalGraphProcessingContext;

class SignalSource{
    friend class SignalSink;

    int m_index = 0;
    int m_connected_sinks = 0;
    int m_processed_sinks = 0;

public:
    SignalSource();

    ~SignalSource();
};


class SignalSink{
    SignalSource* m_connected_source = nullptr;

public:
    SignalSink();

    ~SignalSink();

    void setConnectedSource(SignalSource* source);
};


class SignalGraphNode : public LinkedList<SignalGraphNode>::Node{
    friend class SignalGraph;

    unsigned int m_position = 0;
    int m_size = 0;

public:
    SignalGraphNode();

    virtual ~SignalGraphNode();

    virtual void process(SignalGraphProcessingContext* ctx) = 0;

    inline void setSize(int size) { m_size = size; }

    inline int size() const { return m_size; }

    virtual void getSources(SignalSource* &sources, int &nsources);

    virtual void getSinks(SignalSink* &sinks, int &nsinks);
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


class SignalGraph{
    LinkedList<SignalGraphNode> m_nodes;

public:
    void process(SignalGraphProcessingContext* sgpctx);

    void insertNode(SignalGraphNode* node, SignalGraphNode* after = nullptr, SignalGraphNode* before = nullptr);

    void removeNode(SignalGraphNode* node);

    void sort();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
