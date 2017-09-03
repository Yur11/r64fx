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

    virtual void prologue() = 0;

    virtual void routine(int i) = 0;

    virtual void epilogue() = 0;
};


typedef IteratorPair<LinkedList<SignalGraphElement>::Iterator> SignalGraphElementIterators;


class SignalPort{
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

    long m_size = 0;

public:
    SignalGraphNode(long size);

    virtual ~SignalGraphNode();

    inline long size() const { return m_size; }

    virtual void forEachPort(bool (*fun)(SignalGraphNode* node, SignalPort* port, void* arg), void* arg) = 0;
};


class SignalGraphEdge : public SignalGraphElement{
    SignalSource*  m_source         = nullptr;
    SignalSink*    m_sink           = nullptr;
    int            m_size           = 0;
    short          m_source_offset  = 0;
    short          m_sink_offset    = 0;

public:
    SignalGraphEdge(SignalSource* source, SignalSink* sink);

    virtual ~SignalGraphEdge();
};


class SignalGraph{
    LinkedList<SignalGraphElement> m_elements;

public:
    void add(SignalGraphElement* element) { m_elements.append(element); }

    inline void removeElement(SignalGraphElement* element) { m_elements.remove(element); };

    inline SignalGraphElementIterators elements() const { return {m_elements.begin(), m_elements.end()}; }
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
