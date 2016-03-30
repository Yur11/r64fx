#include "SignalNode_AudioIO.hpp"
#include "SignalGraph.hpp"
#include <assert.h>
#include <iostream>

using namespace std;

namespace r64fx{


SignalNodeClass_AudioIO::SignalNodeClass_AudioIO(SignalGraph* parent_graph)
: SignalNodeClass(parent_graph)
{

}


void SignalNodeClass_AudioIO::reallocateBuffers()
{
    freeBuffers();

    m_size = totalSlotCount();
    port()->resize(m_size);

    m_buffers = new float*[m_size];
    for(int i=0; i<m_size; i++)
    {
        m_buffers[i] = new float[bufferSize()];
    }
}


void SignalNodeClass_AudioIO::freeBuffers()
{
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


SignalNode* SignalNodeClass_AudioIO::newNode(const std::string &name, int slot_count)
{
    if(slot_count < 0)
        return nullptr;

    auto node = SignalNodeClass::newNode(slot_count);
    if(!node)
        return nullptr;

    void* data = nullptr;

    switch(direction())
    {
        case SignalDirection::Input:
        {
            if(slot_count == 1)
            {
                data = parentGraph()->soundDriver()->newAudioInput(name);
            }
            else
            {
                data = new void*[slot_count];
                auto ports = (SoundDriverIOPort_AudioInput**) data;

                for(int i=0; i<slot_count; i++)
                {
                    ports[i] = parentGraph()->soundDriver()->newAudioInput(name);
                }
            }

            break;
        }

        case SignalDirection::Output:
        {
            if(slot_count == 1)
            {
                data = parentGraph()->soundDriver()->newAudioOutput(name);
            }
            else
            {
                data = new void*[slot_count];
                auto ports = (SoundDriverIOPort_AudioOutput**) data;

                for(int i=0; i<slot_count; i++)
                {
                    ports[i] = parentGraph()->soundDriver()->newAudioOutput(name);
                }
            }

            break;
        }
    }

    setNodeData(node, data);

    return node;
}


void SignalNodeClass_AudioIO::deleteNode(SignalNode* node)
{
    void* data = getNodeData(node);

    if(node->slotCount() == 1)
    {
        auto port = (SoundDriverIOPort*) data;
        delete port;
    }
    else
    {
        auto ports = (SoundDriverIOPort**) data;
        for(int i=0; i<node->slotCount(); i++)
        {
            delete ports[i];
        }
        delete[] ports;
    }
}


SignalNodeClass_AudioInput::SignalNodeClass_AudioInput(SignalGraph* parent_graph)
: SignalNodeClass_AudioIO(parent_graph)
{

}


SignalDirection SignalNodeClass_AudioInput::direction()
{
    return SignalDirection::Input;
}


void SignalNodeClass_AudioInput::prepare()
{
    for(auto node : m_nodes)
    {
        auto inport = (SoundDriverIOPort_AudioInput*) getNodeData(node);
        assert(inport);

        float* buffer = m_buffers[node->slotOffset()];
        assert(buffer);

        inport->readSamples(buffer, bufferSize());
    }
}


void SignalNodeClass_AudioInput::process(int sample)
{
    for(int i=0; i<m_size; i++)
    {
        m_source[i] = m_buffers[i][sample];
    }
}


void SignalNodeClass_AudioInput::finish()
{

}


SignalPort* SignalNodeClass_AudioInput::port()
{
    return &m_source;
}


SignalNodeClass_AudioOutput::SignalNodeClass_AudioOutput(SignalGraph* parent_graph)
: SignalNodeClass_AudioIO(parent_graph)
{

}



SignalDirection SignalNodeClass_AudioOutput::direction()
{
    return SignalDirection::Output;
}


void SignalNodeClass_AudioOutput::prepare()
{

}


void SignalNodeClass_AudioOutput::process(int sample)
{
    for(int i=0; i<m_size; i++)
    {
        m_buffers[i][sample] = m_sink[i];
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

        outport->writeSamples(buffer, bufferSize());
    }
}


SignalPort* SignalNodeClass_AudioOutput::port()
{
    return &m_sink;
}

}//namespace r64fx