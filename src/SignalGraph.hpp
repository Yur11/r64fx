#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "SignalNode.hpp"
#include "SignalNodeConnection.hpp"
#include "SoundDriver.hpp"

namespace r64fx{


class SignalGraph{
    SoundDriver*                       m_driver       = nullptr;
    SoundDriverIOStatusPort*           m_status_port  = nullptr;
    LinkedList<SignalNodeClass>        m_node_classes;
    LinkedList<SignalNodeConnection>  m_connections;

public:
    SignalGraph(SoundDriver* driver);

    void addNodeClass(SignalNodeClass* node_class);

    SignalNodeConnection* newConnection(SignalSink* sink, SignalNode* dst_node, SignalSource* source, SignalNode* src_node);

public:
    bool process();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP