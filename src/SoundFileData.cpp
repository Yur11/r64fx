#include "SoundFileData.hpp"
#include <new>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#define R64FX_OWNS_DATA 1

namespace r64fx{

SoundFileData::SoundFileData(int frame_count, int component_count, float* data, bool copy_data)
{
    load(frame_count, component_count, data, copy_data);
}


SoundFileData::~SoundFileData()
{
    free();
}


void SoundFileData::load(int frame_count, int component_count, float* data, bool copy_data)
{
    free();

    if(frame_count > 0 && component_count > 0)
    {
        int size = (frame_count + 1) * component_count;
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
        m_component_count = component_count;
    }
}


void SoundFileData::free()
{
    if(m_data && SoundFileData::ownsData())
    {
        delete[] m_data;
    }

    m_data = nullptr;
    m_frame_count = 0;
    m_component_count = 0;
    m_flags = 0;
}


bool SoundFileData::ownsData() const
{
    return m_flags & R64FX_OWNS_DATA;
}


float* SoundFileData::data() const
{
    return m_data;
}


int SoundFileData::frameCount() const
{
    return m_frame_count;
}


int SoundFileData::componentCount() const
{
    return m_component_count;
}


float* SoundFileData::frame(int i) const
{
#ifdef R64FX_DEBUG
    assert(i < m_frame_count);
#endif//R64FX_DEBUG
    return m_data + (i * m_component_count);
}


void SoundFileData::setSampleRate(int rate)
{
    m_sample_rate = rate;
}


int SoundFileData::sampleRate() const
{
    return m_sample_rate;
}


bool SoundFileData::isGood() const
{
    return m_data && m_frame_count > 0 && m_component_count > 0;
}

}//namespace r64fx
