#ifndef R64FX_SIGNAL_NODE_CONTROLLER_HPP
#define R64FX_SIGNAL_NODE_CONTROLLER_HPP

#include "SignalNode.hpp"
#include "Midi.hpp"

namespace r64fx{

class SignalNode_Controller : public SignalNode{

};


class SignalNodeClass_Controller : public SignalNodeClass{
    SignalSource m_value;

public:
    SignalNodeClass_Controller(SignalGraph* parent_graph);

    inline SignalSource* value() { return &m_value; }

    SignalNode_Controller* newNode(const std::string &port_name, int midi_chan, int controller_number);

    void deleteNode(SignalNode_Controller* node);

protected:
    virtual void process(int sample);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_CONTROLLER_HPP