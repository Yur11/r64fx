#ifndef R64FX_MIDI_DISPATCHER_HPP
#define R64FX_MIDI_DISPATCHER_HPP

#include "Midi.hpp"
#include "LinkedList.hpp"

namespace r64fx{

typedef void (*MidiDispatcherCallback)(const MidiEvent &event, void* arg);

class MidiDispatcherRule : public LinkedList<MidiDispatcherRule>::Node{
    MidiDispatcherCallback m_dispatch;
    void* m_arg;

public:
    MidiDispatcherRule(MidiDispatcherCallback callback, void* arg);

    virtual bool match(void* port, const MidiEvent &event) = 0;

    void dispatch(const MidiEvent &event);
};


class MidiDispatcherRule_CC_Port_Chan_Num : public MidiDispatcherRule{
    void* m_port;
    int   m_chan;
    int   m_num;

public:
    MidiDispatcherRule_CC_Port_Chan_Num(void* port, int chan, int num, MidiDispatcherCallback callback, void* arg);

    virtual bool match(void* port, const MidiEvent &event);
};


class MidiDispatcher{
    LinkedList<MidiDispatcherRule> m_rules;

public:
    void addRule(MidiDispatcherRule* rule);

    MidiDispatcherRule* removeRule(MidiDispatcherRule* rule);

    void dispatch(void* port, const MidiEvent &event);
};

}//namespace r64fx

#endif//R64FX_MIDI_DISPATCHER_HPP