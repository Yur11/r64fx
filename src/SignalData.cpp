#include "SignalData.hpp"


namespace r64fx{

SignalData::SignalData()
{

}


SignalData::SignalData(int size, int chan_count)
{
    alloc(size, chan_count);
}


SignalData::~SignalData()
{
    free();
}


void SignalData::alloc(int size, int chan_count)
{
    if(chan_count <= 0)
        return;
    else if(chan_count == 1)
    {
        m_data = new float[size];
    }
    else
    {
        auto chan = new float*[chan_count];
        for(int i=0; i<chan_count; i++)
        {
            chan[i] = new float[size];
        }
    }

    m_size       = size;
    m_chan_count = chan_count;
}


void SignalData::free()
{
    if(m_chan_count <= 0)
        return;
    else if(m_chan_count == 1)
    {
        auto data = (float*) m_data;
        delete[] data;
    }
    else for(int i=0; i<m_chan_count; i++)
    {
        auto chan = (float**) m_data;
        delete[] chan[i];
    }

    m_data       = nullptr;
    m_size       = 0;
    m_chan_count = 0;
}


int SignalData::size() const
{
    return m_size;
}


int SignalData::channelCount() const
{
    return m_chan_count;
}


float* SignalData::data(int chan) const
{
    if(m_chan_count <= 0 || chan >= m_chan_count)
        return nullptr;
    else if(m_chan_count == 1)
        return (float*) m_data;
    else
        return ((float**) m_data)[chan];
}

}//namespace r64fx