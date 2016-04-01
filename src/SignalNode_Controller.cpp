#include "SignalNode_Controller.hpp"

namespace r64fx{

SignalNodeClass_Controller::SignalNodeClass_Controller(SignalGraph* parent_graph)
: SignalNodeClass(parent_graph)
, m_value("value", 0.0)
{

}


void SignalNodeClass_Controller::process(int sample)
{

}


SignalNode_Controller* SignalNodeClass_Controller::newNode(const std::string &port_name, int midi_chan, int controller_number)
{
    SignalNode_Controller* node = new(std::nothrow) SignalNode_Controller;
    addNewNode(node);
    return node;
}


void SignalNodeClass_Controller::deleteNode(SignalNode_Controller* node)
{
    SignalNodeClass::deleteNode(node);
}

}//namespace r64fx