#include "SignalNode_Oscillator.hpp"
#include <cmath>

namespace r64fx{

SignalNodeClass_Oscillator::SignalNodeClass_Oscillator(SignalGraph* parent_graph)
: SignalNodeClass(parent_graph)
, m_frequency ("frequncy", 440.0f)
, m_period    ("period",   0.0f)
, m_sine      ("sine",     0.0f)
{

}


void SignalNodeClass_Oscillator::forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg)
{
    fun(&m_frequency, arg);
    fun(&m_period,    arg);
    fun(&m_sine,      arg);
}


void SignalNodeClass_Oscillator::process(int sample)
{
    for(int i=0; i<size(); i++)
    {
        float* freq  = m_frequency.buffer() + i;
        float delta = freq[0] * sampleRateReciprocal();

        float* period = m_period.buffer() + i;
        period[0] += delta;
        if(period[0] >= 1.0)
            period[0] -= 1.0f;

        float* sine = m_sine.buffer() + i;
        sine[0] = sin(period[0] * 2.0f * M_PI);
    }
}

}//namespace r64fx
