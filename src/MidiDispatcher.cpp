#include "MidiDispatcher.hpp"

namespace r64fx{

MidiDispatcherRule::MidiDispatcherRule(MidiDispatcherCallback callback, void* arg)
: m_dispatch(callback)
, m_arg(arg)
{

}


void MidiDispatcherRule::dispatch(const MidiEvent &event)
{
    m_dispatch(event, m_arg);
}


MidiDispatcherRule_CC_Port_Chan_Num::MidiDispatcherRule_CC_Port_Chan_Num
(void* port, int chan, int num, MidiDispatcherCallback callback, void* arg)
: MidiDispatcherRule(callback, arg)
, m_port(port)
, m_chan(chan)
, m_num(num)
{

}


bool MidiDispatcherRule_CC_Port_Chan_Num::match(void* port, const MidiEvent &event)
{
    return m_port == port && m_chan == event.channel() && m_num == event.controllerNumber();
}


void MidiDispatcher::addRule(MidiDispatcherRule* rule)
{
    m_rules.append(rule);
}


MidiDispatcherRule* MidiDispatcher::removeRule(MidiDispatcherRule* rule)
{
    m_rules.remove(rule);
    return rule;
}


void MidiDispatcher::dispatch(void* port, const MidiEvent &event)
{
    for(auto rule : m_rules)
    {
        if(rule->match(port, event))
        {
            rule->dispatch(event);
        }
    }
}

}//namespace r64fx
