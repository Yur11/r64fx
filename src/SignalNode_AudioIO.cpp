#include "SignalNode_AudioIO.hpp"

namespace r64fx{

SignalNodeClass_AudioIO::SignalNodeClass_AudioIO(SoundDriver* driver)
: m_driver(driver)
{

}


void SignalNodeClass_AudioIO::reallocateBuffers()
{
    freeBuffers();

    m_ports = new float[m_size];
    m_buffers = new float*[m_size];
    for(int i=0; i<m_size; i++)
    {
        m_buffers[i] = new float[m_driver->bufferSize()];
    }
}


void SignalNodeClass_AudioIO::freeBuffers()
{
    if(m_ports)
    {
        delete m_ports;
        m_ports = nullptr;
    }

    if(m_buffers)
    {
        for(int i=0; i<m_size; i++)
        {
            float* &buffer = m_buffers[i];
            if(buffer)
            {
                delete buffer;
                buffer = nullptr;
            }
        }
        delete m_buffers;
        m_buffers = nullptr;
    }
}


void SignalNodeClass_AudioIO::nodeAppended(SignalNode* node)
{
    reallocateBuffers();
}


void SignalNodeClass_AudioIO::nodeRemoved(SignalNode* node)
{
    reallocateBuffers();
}


void SignalNodeClass_AudioIO::prepare()
{

}


void SignalNodeClass_AudioInput::process(int sample)
{
    for(int i=0; i<m_size; i++)
    {
        m_ports[i] = m_buffers[i][sample];
    }
}


void SignalNodeClass_AudioInput::finish()
{

}


void SignalNodeClass_AudioOutput::process(int sample)
{
    for(int i=0; i<m_size; i++)
    {
        m_buffers[i][sample] = m_ports[i];
    }
}


void SignalNodeClass_AudioOutput::finish()
{

}

}//namespace r64fx