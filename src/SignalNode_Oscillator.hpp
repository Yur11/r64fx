#ifndef R64FX_SIGNAL_NODE_OSCILLATOR_HPP
#define R64FX_SIGNAL_NODE_OSCILLATOR_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_Oscillator : public SignalNodeClass{
    SignalSink   m_frequency;
    SignalSource m_period;
    SignalSource m_sine;

public:
    SignalNodeClass_Oscillator(SignalGraph* parent_graph);

    inline SignalSink* frequency() { return &m_frequency; }

    inline SignalSource* period() { return &m_period; }

    inline SignalSource* sine() { return &m_sine; }

    void forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg);

protected:
    virtual void process(int sample);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_OSCILLATOR_HPP