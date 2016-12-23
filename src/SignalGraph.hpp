#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SignalNode;

class SignalPort{
    float*      m_addr   = nullptr;
    SignalNode* m_parent = nullptr;

public:
    SignalPort(float* addr, SignalNode* parent = nullptr) 
    : m_addr(addr) 
    , m_parent(parent)
    {}

    SignalPort() {}

    inline SignalNode* parentNode() const { return m_parent; }

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


class SignalEdge : public LinkedList<SignalEdge>::Node{
    SignalNode*   m_parent = nullptr;
    SignalSource* m_source = nullptr;
    SignalSink*   m_sink   = nullptr;

public:
    SignalEdge(SignalSource* source, SignalSink* sink, SignalNode* parent = nullptr);

    void setParent(SignalNode* node);

    SignalNode* parent() const;
};


class SignalNode : public LinkedList<SignalNode>::Node{
    friend class SignalEdge;
    SignalNode* m_parent = nullptr;
    LinkedList<SignalNode> m_nodes;
    LinkedList<SignalEdge> m_edges;

public:
    void setParent(SignalNode* node);

    SignalNode* parent() const;

    IteratorPair<LinkedList<SignalNode>::Iterator> nodes() const;

    IteratorPair<LinkedList<SignalEdge>::Iterator> edges() const;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
