#include "SignalNode_Filter.hpp"

namespace r64fx{

SignalNode_Filter::SignalNode_Filter()
{
    setSize(1);
}


SignalNode_Filter::~SignalNode_Filter()
{

}


void SignalNode_Filter::setSize(int size)
{
    if(size == m_size)
        return;

    m_size = size;

    if(m_sink)
    {
        float* buffer = m_sink->addr();
        if(buffer)
        {
            delete[] buffer;
        }
        delete[] m_sink;
        m_sink = nullptr;
    }

    if(m_size > 0)
    {
        m_sink = new SignalSink[size];
        float* buffer = new float[size];
        for(int i=0; i<m_size; i++)
        {
            m_sink[i].setAddr(buffer + i);
        }
    }

    if(m_source)
    {
        float* buffer = m_source->addr();
        if(buffer)
        {
            delete[] buffer;
        }
        delete[] m_source;
        m_source = nullptr;
    }

    if(m_size > 0)
    {
        m_source = new SignalSource[size];
        float* buffer = new float[size];
        for(int i=0; i<m_size; i++)
        {
            m_source[i].setAddr(buffer + i);
        }
    }

    if(m_prev_input)
    {
        delete[] m_prev_input;
        m_prev_input = nullptr;
    }

    if(m_size)
    {
        m_prev_input = new float[m_size];
        for(int i=0; i<m_size; i++)
        {
            m_prev_input[i] = 0.0f;
        }
    }
}


int SignalNode_Filter::size() const
{
    return m_size;
}


void SignalNode_Filter::setPole(float pole)
{
    m_pole = pole;
}


SignalSink* SignalNode_Filter::sink(int i) const
{
    return m_sink + i;
}

SignalSource* SignalNode_Filter::source(int i) const
{
    return m_source + i;
}


void SignalNode_Filter::processSample(int i)
{
    if(m_size <= 0)
        return;

    for(int i=0; i<m_size; i++)
    {
        m_source[i][0] = -(m_prev_input[i] * (1.0f + m_pole) * 0.5) + (m_sink[i][0] * (1.0f + m_pole) * 0.5) + m_source[i][0] * m_pole;
        m_prev_input[i] = m_sink[i][0];
    }
}

}//namespace r64fx