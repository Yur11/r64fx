#ifndef R64FX_SIGNAL_NODE_CONNECTION_HPP
#define R64FX_SIGNAL_NODE_CONNECTION_HPP

#include "LinkedList.hpp"


namespace r64fx{

class SignalNode;
class SignalSink;
class SignalPort;

class SignalNodeConnection : public LinkedList<SignalNodeConnection>::Node{
    enum class RelationType{
        Matching,
        OneToAll,
        AllToOneMixed
    };

public:
    friend class SignalGraph;

    SignalNode* m_dst = nullptr;
    SignalNode* m_src = nullptr;

    SignalSink* m_dst_port = nullptr;
    SignalPort* m_src_port = nullptr;

    RelationType m_relation_type = RelationType::Matching;

    SignalNodeConnection(SignalNode* dst, SignalSink* dst_port, SignalNode* src, SignalPort* src_port);

    void process();

public:
    RelationType relationType() const;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_CONNECTION_HPP