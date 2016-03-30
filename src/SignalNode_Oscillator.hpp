#ifndef R64FX_SIGNAL_NODE_OSCILLATOR_HPP
#define R64FX_SIGNAL_NODE_OSCILLATOR_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_Oscillator : public SignalNodeClass{
    SignalSink   m_frequency;
    SignalSource m_period;
    SignalSource m_sine;
    int          m_size = 0;

public:
    SignalNodeClass_Oscillator(SignalGraph* parent_graph);

    inline SignalSink* frequency() { return &m_frequency; }

    inline SignalSource* period() { return &m_period; }

    inline SignalSource* sine() { return &m_sine; }

protected:
    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

    virtual void nodeAppended(SignalNode* node);

    virtual void nodeRemoved(SignalNode* node);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_OSCILLATOR_HPP