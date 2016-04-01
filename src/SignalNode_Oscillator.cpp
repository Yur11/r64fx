#include "SignalNode_Oscillator.hpp"
#include <cmath>

namespace r64fx{

SignalNodeClass_Oscillator::SignalNodeClass_Oscillator(SignalGraph* parent_graph)
: SignalNodeClass(parent_graph)
{

}


void SignalNodeClass_Oscillator::prepare()
{

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


void SignalNodeClass_Oscillator::finish()
{

}


void SignalNodeClass_Oscillator::nodeAppended(SignalNode* node)
{
    m_frequency.resize(size());
    m_period.resize(size());
    m_sine.resize(size());
}


void SignalNodeClass_Oscillator::nodeRemoved(SignalNode* node)
{
    m_frequency.resize(size());
    m_period.resize(size());
    m_sine.resize(size());
}

}//namespace r64fx
