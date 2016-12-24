#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SignalNode;

/* Base class for Nodes and edges. */
class SignalGraphElement : public LinkedList<SignalGraphElement>::Node{
    SignalNode* m_parent = nullptr;

public:
    virtual ~SignalGraphElement();

    void setParent(SignalNode* parent);

    SignalNode* parent() const;

protected:
    virtual void prologue() = 0;

    virtual void routine(int i) = 0;

    virtual void epilogue() = 0;
};


class SignalPort{
    float*      m_addr   = nullptr;
    SignalNode* m_parent = nullptr;

public:
    SignalPort(SignalNode* parent = nullptr);

    ~SignalPort();

    inline SignalNode* parentNode() const { return m_parent; }

    inline float &operator[](unsigned long index) { return m_addr[index]; }
};


class SignalSource : public SignalPort{
public:
    using SignalPort::SignalPort;
};


class SignalSink : public SignalPort{
public:
    using SignalPort::SignalPort;    
};


class SignalNode : public SignalGraphElement{
    friend class SignalGraphElement;
    LinkedList<SignalGraphElement> m_subgraph;

public:
    SignalNode();

    virtual ~SignalNode();

protected:
    virtual void prologue();

    virtual void routine(int i);

    virtual void epilogue();
};


class SignalEdge : public SignalGraphElement{
    SignalNode*   m_parent = nullptr;
    SignalSource* m_source = nullptr;
    SignalSink*   m_sink   = nullptr;

public:
    SignalEdge(SignalSource* source, SignalSink* sink, SignalNode* parent = nullptr);

    virtual ~SignalEdge();

private:
    virtual void routine(int i);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
