#ifndef R64FX_SIGNAL_NODE_OSCILLATOR_HPP
#define R64FX_SIGNAL_NODE_OSCILLATOR_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_Oscillator : public SignalNodeClass{
    SoundDriver* m_driver = nullptr;
    SignalSink   m_frequency;
    SignalSource m_period;
    SignalSource m_sine;
    int          m_size = 0;

public:
    SignalNodeClass_Oscillator(SoundDriver* driver);

    inline SignalSink* frequency() { return &m_frequency; }

    inline SignalSource* period() { return &m_period; }

    inline SignalSource* sine() { return &m_sine; }

    virtual void prepare();

    virtual void process(int sample);

    virtual void finish();

protected:
    virtual void nodeAppended(SignalNode* node);

    virtual void nodeRemoved(SignalNode* node);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_OSCILLATOR_HPP