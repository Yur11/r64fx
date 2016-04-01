#ifndef R64FX_SIGNAL_NODE_CONTROLLER_HPP
#define R64FX_SIGNAL_NODE_CONTROLLER_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"
#include "MidiDispatcher.hpp"
#include <vector>

namespace r64fx{

class SignalNode_Controller : public SignalNode{

};


class SignalNodeClass_Controller : public SignalNodeClass{
    SignalSource m_value;
    std::vector<SoundDriverIOPort_MidiInput*> m_midi_inputs;
    std::vector<std::vector<MidiEvent>> m_midi_cc_events;
    MidiDispatcher m_midi_dispatcher;

public:
    SignalNodeClass_Controller(SignalGraph* parent_graph);

    SignalNode_Controller* newNode(const std::string &name, int midi_chan, int controller_number);

    void deleteNode(SignalNode_Controller* node);

    inline SignalSource* value() { return &m_value; }

    virtual void forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg);

protected:
    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

    SoundDriverIOPort_MidiInput* findInput(const std::string &name);

    void processMidiControlChange(const MidiEvent &event, SignalNode_Controller* node);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_CONTROLLER_HPP