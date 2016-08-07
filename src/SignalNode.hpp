#ifndef R64FX_SIGNAL_NODE_HPP
#define R64FX_SIGNAL_NODE_HPP

#include "LinkedList.hpp"
#include "SignalPort.hpp"

namespace r64fx{

class SignalGraph;
class SignalNodeClass;
    
class SignalNode : public LinkedList<SignalNode>::Node{
public:
    SignalNode(SignalGraph* parent_graph);
    
    virtual ~SignalNode();
    
    virtual SignalNodeClass* nodeClass() = 0;
};


class SignalNodeClass : public LinkedList<SignalNodeClass>::Node{
public:
    virtual void processSample() = 0;
    
    virtual void renderCode() = 0;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_HPP