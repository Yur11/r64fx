#include "SignalPort.hpp"

namespace r64fx{

SignalPort::SignalPort(int size)
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


float &SignalPort::operator[](int i) const
{
    return m_buffer[i];
}


float* SignalPort::buffer() const
{
    return m_buffer;
}


SignalSink::SignalSink(int size)
: SignalPort(size)
{

}


SignalSource::SignalSource(int size)
: SignalPort(size)
{

}

}//namespace r64fx