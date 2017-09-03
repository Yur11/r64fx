#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SignalGraphNode;

/* Base class for Nodes and Edges. */
class SignalGraphElement : public LinkedList<SignalGraphElement>::Node{

public:
    SignalGraphElement();

    virtual ~SignalGraphElement();

    virtual void routine(int i) = 0;
};


typedef IteratorPair<LinkedList<SignalGraphElement>::Iterator> SignalGraphElementIterators;


class SignalPort{
    friend class SignalGraphNode;
    friend class SignalGraphEdge;
    friend class SignalSource;
    friend class SignalSink;

    float*         m_ptr   = nullptr;
    unsigned long  m_bits  = 0;

    SignalPort(SignalGraphNode* parent);

    ~SignalPort();

public:
    SignalGraphNode* parentNode() const;

    bool isSource() const;

    inline bool isSink() const { return !isSource(); }

    inline float &operator[](int i) { return m_ptr[i]; }
};


class SignalSource : public SignalPort{
public:
    SignalSource(SignalGraphNode* parent);

    ~SignalSource();
};


class SignalSink : public SignalPort{
public:
    SignalSink(SignalGraphNode* parent);

    ~SignalSink();
};


class SignalGraphNode : public SignalGraphElement{
    friend class SignalGraphElement;
    int m_size = 0;

public:
    SignalGraphNode();

    virtual ~SignalGraphNode();

    virtual void forEachPort(bool (*fun)(SignalGraphNode* node, SignalPort* port, void* arg), void* arg) = 0;

    virtual int portCount() = 0;

    void resize(int new_size);

    inline int size() const { return m_size; }
};


class SignalGraphEdge : public SignalGraphElement{
    SignalSource*  m_source         = nullptr;
    SignalSink*    m_sink           = nullptr;
    short          m_source_offset  = 0;
    short          m_sink_offset    = 0;
    int            m_size           = 0;

public:
    SignalGraphEdge(SignalSource* source, short source_offset, SignalSink* sink, short sink_offset, int size = 1);

    virtual ~SignalGraphEdge();

    virtual void routine(int i);

    inline short sourceOffset() const { return m_source_offset; }

    inline short sinkOffset() const { return m_sink_offset; }
};


class SignalGraph{
    LinkedList<SignalGraphElement> m_elements;

public:
    void add(SignalGraphNode* node);

    void add(SignalGraphEdge* edge);

    void remove(SignalGraphNode* node);

    void remove(SignalGraphEdge* edge);

    void run(int nsamples);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
