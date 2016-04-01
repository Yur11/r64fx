#include "SignalData.hpp"
#include <new>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#define R64FX_OWNS_DATA 1

namespace r64fx{

SignalData::SignalData(int frame_count, int channel_count, float* data, bool copy_data)
{

}


SignalData::~SignalData()
{

}


void SignalData::load(int frame_count, int channel_count, float* data, bool copy_data)
{
    free();

    if(frame_count > 0 && channel_count > 0)
    {
        int size = frame_count * channel_count;
        if(data)
        {
            if(copy_data)
            {
                m_data = new (std::nothrow) float[size];
                for(int i=0; i<size; i++)
                {
                    m_data[i] = data[i];
                }
                m_flags |= R64FX_OWNS_DATA;
            }
            else
            {
                m_data = data;
                m_flags &= ~R64FX_OWNS_DATA;
            }
        }
        else
        {
            m_data = new (std::nothrow) float[size];
            m_flags |= R64FX_OWNS_DATA;
        }
        m_frame_count = frame_count;
        m_component_count = channel_count;
    }
}


void SignalData::free()
{
    if(m_data && SignalData::ownsData())
    {
        delete[] m_data;
    }

    m_data = nullptr;
    m_frame_count = 0;
    m_component_count = 0;
    m_flags = 0;
}


bool SignalData::ownsData() const
{
    return m_flags & R64FX_OWNS_DATA;
}


float* SignalData::data() const
{
    return m_data;
}


int SignalData::frameCount() const
{
    return m_frame_count;
}


int SignalData::componentCount() const
{
    return m_component_count;
}


float* SignalData::frame(int i) const
{
#ifndef R64FX_DEBUG
    assert(i < frame_count);
#endif//R64FX_DEBUG
    return m_data + (i * m_component_count);
}


void SignalData::setSampleRate(int rate)
{
    m_sample_rate = rate;
}


int SignalData::sampleRate() const
{
    return m_sample_rate;
}

}//namespace r64fx