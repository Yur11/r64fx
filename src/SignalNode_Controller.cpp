#include "SignalNode_Controller.hpp"
#include "SoundDriver.hpp"
#include <iostream>

using namespace std;

namespace r64fx{

SignalNodeClass_Controller::SignalNodeClass_Controller(SignalGraph* parent_graph)
: SignalNodeClass(parent_graph)
, m_value("value", 0.0)
{

}


SignalNode_Controller* SignalNodeClass_Controller::newNode(const std::string &name, int midi_chan, int controller_number)
{
    bool created_new_port = false;
    auto driver = soundDriver();

    SignalNode_Controller* node = new(std::nothrow) SignalNode_Controller;
    if(!node)
        return nullptr;

    auto midi_input = findInput(name);
    if(!midi_input)
    {
        midi_input = driver->newMidiInput(name);
        if(!midi_input)
        {
            cerr << "Failed to create new midi input \"" << name << "\"!\n";
            delete node;
            return nullptr;
        }
        m_midi_inputs.push_back(midi_input);
        created_new_port = true;
    }

    auto rule = new(std::nothrow) MidiDispatcherRule_CC_Port_Chan_Num(
        midi_input, midi_chan, controller_number,
        [](const MidiEvent &event, void* arg){
            auto node = (SignalNode_Controller*) arg;
            auto node_class = (SignalNodeClass_Controller*) node->parentClass();
            node_class->processMidiControlChange(event, (SignalNode_Controller*)arg);
        },
        node
    );

    if(!rule)
    {
        cerr << "Failed to allocate new midi dispatch rule for \"" << name << "\"!";
        delete node;
        if(created_new_port)
        {
            driver->deletePort(midi_input);
        }
        return nullptr;
    }
    m_midi_dispatcher.addRule(rule);

    addNewNode(node);
    m_midi_cc_events.resize(size());

    return node;
}


void SignalNodeClass_Controller::deleteNode(SignalNode_Controller* node)
{
    m_midi_cc_events.erase(m_midi_cc_events.begin() + node->slotOffset());
    SignalNodeClass::deleteNode(node);
}


void SignalNodeClass_Controller::forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg)
{
    fun(&m_value, arg);
}


void SignalNodeClass_Controller::prepare()
{
    for(auto input : m_midi_inputs)
    {
        MidiEvent event;
        while(input->readEvents(&event, 1))
        {
            m_midi_dispatcher.dispatch(input, event);
        }
    }
}


void SignalNodeClass_Controller::process(int sample)
{
    for(int i=0; i<size(); i++)
    {
        auto &events = m_midi_cc_events[i];
        for(auto &event : events)
        {
            if(event.time() == sample)
            {
                float value = float(event.message().controllerValue()) / 128.0f;
                m_value.buffer()[i] = value;
                cout << "cc: " << value << "\n";
            }
        }
    }
}


void SignalNodeClass_Controller::finish()
{
    for(auto &events : m_midi_cc_events)
    {
        events.clear();
    }
}


SoundDriverIOPort_MidiInput* SignalNodeClass_Controller::findInput(const std::string &name)
{
    SoundDriverIOPort_MidiInput* input = nullptr;
    for(int i=0; i<size(); i++)
    {
        if(m_midi_inputs[i]->name() == name)
        {
            input = m_midi_inputs[i];
            break;
        }
    }
    return input;
}


void SignalNodeClass_Controller::processMidiControlChange(const MidiEvent &event, SignalNode_Controller* node)
{
    m_midi_cc_events[node->slotOffset()].push_back(event);
}

}//namespace r64fx