#ifndef R64FX_SIGNAL_NODE_FILTER_HPP
#define R64FX_SIGNAL_NODE_FILTER_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_Filter : public SignalNode{
    SignalSink*    m_sink    = nullptr;
    SignalSource*  m_source  = nullptr;

    int    m_size  = 0;
    float  m_pole  = 0.0f;
    float* m_prev_input = nullptr;

public:
    SignalNode_Filter();

    virtual ~SignalNode_Filter();

    void setSize(int size);

    int size() const;

    void setPole(float pole);

    SignalSink* sink(int i) const;

    SignalSource* source(int i) const;

private:
    virtual void processSample(int i);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_FILTER_HPP