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
        case SignalDirection::Input:
        {
            setNodeData(node, parentGraph()->soundDriver()->newAudioInput(name), 0);
            break;
        }

        case SignalDirection::Output:
        {
            setNodeData(node, parentGraph()->soundDriver()->newAudioOutput(name), 0);
        }

        default:
            break;
    }

    return node;
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
        auto inport = (SoundDriverIOPort_AudioInput*) getNodeData(node, 0);
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
        auto outport = (SoundDriverIOPort_AudioOutput*) getNodeData(node, 0);
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