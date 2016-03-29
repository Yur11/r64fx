#include "SignalNode_AudioIO.hpp"
#include <assert.h>
#include <iostream>

using namespace std;

namespace r64fx{

SignalNodeClass_AudioIO::SignalNodeClass_AudioIO(SoundDriver* driver)
: m_driver(driver)
{

}


void SignalNodeClass_AudioIO::reallocateBuffers()
{
    freeBuffers();

    m_size = totalSlotCount();

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
    assert(node->slotCount() == 1);
    reallocateBuffers();
}


void SignalNodeClass_AudioIO::nodeRemoved(SignalNode* node)
{
    assert(node->slotCount() == 1);
    reallocateBuffers();
}


SignalNode* SignalNodeClass_AudioIO::newNode(const std::string &name, int slot_count)
{
    auto node = SignalNodeClass::newNode(slot_count);
    if(!node)
        return nullptr;

    switch(direction())
    {
        case SoundDriverIOPort::Direction::Input:
        {
            setNodeData(node, m_driver->newAudioInput(name));
            break;
        }

        case SoundDriverIOPort::Direction::Output:
        {
            setNodeData(node, m_driver->newAudioOutput(name));
        }

        default:
            break;
    }

    return node;
}


SignalNodeClass_AudioInput::SignalNodeClass_AudioInput(SoundDriver* driver)
: SignalNodeClass_AudioIO(driver)
{

}


SoundDriverIOPort::Direction SignalNodeClass_AudioInput::direction()
{
    return SoundDriverIOPort::Direction::Input;
}


void SignalNodeClass_AudioInput::prepare()
{
    for(auto node : m_nodes)
    {
        auto inport = (SoundDriverIOPort_AudioInput*) getNodeData(node);
        assert(inport);

        float* buffer = m_buffers[node->slotOffset()];
        assert(buffer);

        inport->readSamples(buffer, m_driver->bufferSize());
    }
}


void SignalNodeClass_AudioInput::process(int sample)
{
    cout << "SignalNodeClass_AudioInput::process " << m_size << "\n";
    for(int i=0; i<m_size; i++)
    {
        m_ports[i] = m_buffers[i][sample];
    }
}


void SignalNodeClass_AudioInput::finish()
{

}


SignalNodeClass_AudioOutput::SignalNodeClass_AudioOutput(SoundDriver* driver)
: SignalNodeClass_AudioIO(driver)
{

}



SoundDriverIOPort::Direction SignalNodeClass_AudioOutput::direction()
{
    return SoundDriverIOPort::Direction::Output;
}


void SignalNodeClass_AudioOutput::prepare()
{

}


void SignalNodeClass_AudioOutput::process(int sample)
{
    cout << "SignalNodeClass_AudioOutput::process " << m_size << "\n";
    for(int i=0; i<m_size; i++)
    {
        m_buffers[i][sample] = m_ports[i];
    }
}


void SignalNodeClass_AudioOutput::finish()
{
    for(auto node : m_nodes)
    {
        auto outport = (SoundDriverIOPort_AudioOutput*) getNodeData(node);
        assert(outport);

        float* buffer = m_buffers[node->slotOffset()];
        assert(buffer);

        outport->writeSamples(buffer, m_driver->bufferSize());
    }
}

}//namespace r64fx