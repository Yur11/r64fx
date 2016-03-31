#include "SignalPort.hpp"

namespace r64fx{

SignalPort::SignalPort(const std::string name, int size)
: m_name(name)
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


SignalSink::SignalSink(const std::string name, int size)
: SignalPort(name, size)
{

}


SignalSource::SignalSource(const std::string name, int size)
: SignalPort(name, size)
{

}

}//namespace r64fx