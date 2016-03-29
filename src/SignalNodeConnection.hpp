#ifndef R64FX_SIGNAL_NODE_CONNECTION_HPP
#define R64FX_SIGNAL_NODE_CONNECTION_HPP

#include "LinkedList.hpp"

namespace r64fx{

class SignalNodeConnection : public LinkedList<SignalNodeConnection>::Node{
    friend class SignalGraph;

    float* m_dst = nullptr;
    float* m_src = nullptr;

    SignalNodeConnection(float* dst, float* src);

    void process();
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_CONNECTION_HPP