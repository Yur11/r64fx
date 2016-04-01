#include "SignalPort.hpp"

namespace r64fx{

SignalPort::SignalPort(const std::string name,  float default_value, int size)
: m_name(name)
, m_default_value(default_value)
{
    resize(size);
}


SignalPort::~SignalPort()
{
    free();
}


void SignalPort::resize(int size)
{
    if(size < 1)
        return;

    free();
    m_buffer = new float[size];
    for(int i=0; i<size; i++)
    {
        m_buffer[i] = defaultValue();
    }
}


void SignalPort::free()
{
    if(m_buffer)
    {
        delete m_buffer;
        m_buffer = nullptr;
    }
}


float &SignalPort::slot(int i) const
{
    return m_buffer[i];
}


float* SignalPort::buffer() const
{
    return m_buffer;
}


SignalSink::SignalSink(const std::string name, float default_value, int size)
: SignalPort(name, default_value, size)
{

}


SignalSource::SignalSource(const std::string name, float default_value, int size)
: SignalPort(name, default_value, size)
{

}

}//namespace r64fx