#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"
#include "LinkedList.hpp"

namespace r64fx{


class SignalGraph{
    SoundDriver*                 m_driver       = nullptr;
    SoundDriverIOStatusPort*     m_status_port  = nullptr;
    LinkedList<SignalNodeClass>  m_node_classes;

public:
    SignalGraph(SoundDriver* driver);

    void addNodeClass(SignalNodeClass* node_class);

public:
    void process();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP