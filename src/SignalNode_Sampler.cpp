#include "SignalNode_Sampler.hpp"
#include <cmath>

#include <iostream>
using namespace std;

#define R64FX_SAMPLER_PLAYING 1
#define R64FX_SAMPLER_LOOPING 2

const float g_playback_rate = 48000.0f;
const float g_playback_rate_rcp = 1.0f / g_playback_rate;

namespace r64fx{

SignalNode_Sampler::SignalNode_Sampler()
{
    resizeOutput(1);
}


SignalNode_Sampler::~SignalNode_Sampler()
{
    clear();
    resizeOutput(0);
}


void SignalNode_Sampler::setData(float* data, int frame_count, int component_count, float sample_rate)
{
    clear();

    if(data && frame_count > 0 && component_count > 0 && sample_rate > 0)
    {
        m_data = data;
        m_frame_count = frame_count;
        if(component_count != m_component_count)
        {
            m_component_count = component_count;
        }
        m_sample_rate = sample_rate;
        m_sample_rate_rcp = 1.0f / m_sample_rate;
        m_start = m_playhead = m_loop_in = 0.0f;
        m_stop = m_loop_out = m_frame_count * m_sample_rate_rcp;
    }
}


void SignalNode_Sampler::clear()
{
    m_data = nullptr;
    m_component_count = 0;
}


int SignalNode_Sampler::componentCount()
{
    return m_component_count;
}


void SignalNode_Sampler::setStartTime(float start)
{
    m_start = start;
}


float SignalNode_Sampler::startPostion() const
{
    return m_start;
}


void SignalNode_Sampler::setStopPostion(float stop)
{
    m_stop = stop;
}


float SignalNode_Sampler::stopTime() const
{
    return m_stop;
}


void SignalNode_Sampler::setLoopInTime(float loop_in)
{
    m_loop_in = loop_in;
}


float SignalNode_Sampler::loopInTime() const
{
    return m_loop_in;
}


void SignalNode_Sampler::setLoopOutTime(float loop_out)
{
    m_loop_out = loop_out;
}


float SignalNode_Sampler::loopOutTime() const
{
    return m_loop_out;
}


void SignalNode_Sampler::setPlayheadTime(float playhead)
{
    m_playhead = playhead;
}


float SignalNode_Sampler::playheadTime() const
{
    return m_playhead;
}


void SignalNode_Sampler::setPitch(float pitch)
{
    m_pitch = pitch;
}


void SignalNode_Sampler::setGain(float gain)
{
    m_gain = gain;
}


float SignalNode_Sampler::gain() const
{
    return m_gain;
}


float SignalNode_Sampler::pitch() const
{
    return m_pitch;
}


void SignalNode_Sampler::play()
{
    m_flags |= R64FX_SAMPLER_PLAYING;
}


void SignalNode_Sampler::stop()
{
    m_flags &= ~R64FX_SAMPLER_PLAYING;
}


bool SignalNode_Sampler::isPlaying() const
{
    return m_flags & R64FX_SAMPLER_PLAYING;
}


void SignalNode_Sampler::enableLoop()
{
    m_flags |= R64FX_SAMPLER_LOOPING;
}


void SignalNode_Sampler::disableLoop()
{
    m_flags &= ~R64FX_SAMPLER_LOOPING;
}


bool SignalNode_Sampler::isLooping() const
{
    return m_flags & R64FX_SAMPLER_LOOPING;
}


SignalSource* SignalNode_Sampler::output(int channel)
{
    return m_output + channel;
}


void SignalNode_Sampler::resizeOutput(int size)
{
    if(size == m_output_size || size < 0)
        return;

    if(m_output)
    {
        float* buffer = m_output->addr();
        if(buffer)
        {
            delete[] buffer;
        }
        delete[] m_output;
        m_output = nullptr;
    }

    if(size > 0)
    {
        m_output = new SignalSource[size];
        float* buffer = new float[size];
        for(int i=0; i<size; i++)
        {
            m_output[i].setAddr(buffer + i);
        }
    }

    m_output_size = size;
}


void SignalNode_Sampler::addedToGraph(SignalGraph* graph)
{

}


void SignalNode_Sampler::aboutToBeRemovedFromGraph(SignalGraph* graph)
{

}


void SignalNode_Sampler::processSample(int i)
{
    if(!m_data)
        return;

    if(!isPlaying())
        return;

    float delta = m_pitch * g_playback_rate_rcp;
    float y = delta - m_playhead_kahan;
    float tmp = m_playhead + y;
    m_playhead_kahan = (tmp - m_playhead) - y;
    m_playhead = tmp;

    if(isLooping())
    {
        if(m_playhead >= m_loop_out)
        {
            m_playhead = m_playhead - m_loop_out + m_loop_in;
        }
    }
    else
    {
        if(m_playhead >= m_stop)
        {
            stop();
            m_playhead = m_stop;
        }
    }

    float playhead_frame = m_playhead * m_sample_rate;
    double whole_part = 0.0;
    float frac_part = modf(playhead_frame, &whole_part);
    int idx = (int)whole_part;

    int count = min(m_component_count, m_output_size);
    for(int c=0; c<count; c++)
    {
        float val1 = m_data[idx * m_component_count + c];
        float val2 = m_data[(idx + 1) * m_component_count + c];
        float delta = val2 - val1;
        float value = val1 + frac_part * delta;
        m_output[c][0] = value * m_gain;
    }
}


}//namespace r64fx