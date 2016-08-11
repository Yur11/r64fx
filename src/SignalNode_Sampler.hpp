#ifndef R64FX_SIGNAL_NODE_SAMPLER_HPP
#define R64FX_SIGNAL_NODE_SAMPLER_HPP

#include "SignalGraph.hpp"

namespace r64fx{
    
class SignalNode_Sampler : public SignalNode{
    float* m_data = nullptr;
    int m_frame_count = 0;
    int m_component_count = 0;
    int m_frame = 0;
    
    SignalSource* m_sources = nullptr;
    
public:
    SignalNode_Sampler(float* data, int frame_count, int component_count, SignalGraph* graph);
    
    virtual ~SignalNode_Sampler();
    
    void setSample(float* data, int frame_count, int component_count);
    
    void clearSample();
    
    int componentCount();
    
    SignalSource* source(int channel);
    
    virtual void processSample(int i);
};
    
}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_SAMPLER_HPP