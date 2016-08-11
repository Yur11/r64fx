#include "SignalNode_Sampler.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
SignalNode_Sampler::SignalNode_Sampler(float* data, int frame_count, int component_count, SignalGraph* graph)
{
    setSample(data, frame_count, component_count);
    setGraph(graph);
    graph->addNode(this);
}
    

SignalNode_Sampler::~SignalNode_Sampler()
{
    clearSample();
}


void SignalNode_Sampler::setSample(float* data, int frame_count, int component_count)
{
    clearSample();
    m_data = data;
    m_frame_count = frame_count;
    m_component_count = component_count;
    m_sources = new SignalSource[component_count];
    float* buff = new float[component_count];
    for(int i=0; i<component_count; i++)
    {
        m_sources[i].setAddr(buff + i);
    }
}


void SignalNode_Sampler::clearSample()
{
    m_data = nullptr;
    m_frame_count = 0;
    m_component_count = 0;
    if(m_sources)
    {
        delete[] m_sources[0].addr();
        delete m_sources;
        m_sources = nullptr;
    }
}


int SignalNode_Sampler::componentCount()
{
    return m_component_count;
}
    
    
SignalSource* SignalNode_Sampler::source(int channel)
{
    return m_sources + channel;
}


void SignalNode_Sampler::processSample(int i)
{
    for(int c=0; c<m_component_count; c++)
    {
        m_sources[c][0] = m_data[m_frame * m_component_count + c];;
    }
    
    m_frame++;
    if(m_frame >= (m_frame_count - 1))
    {
        m_frame = 0;
    }
}
    
}//namespace r64fx