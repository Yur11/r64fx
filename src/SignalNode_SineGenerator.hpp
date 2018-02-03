#ifndef R64FX_SIGNAL_NODE_SINE_GENERATOR_HPP
#define R64FX_SIGNAL_NODE_SINE_GENERATOR_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_SineGenerator : public SignalNode{
    R64FX_NODE_SOURCE(out)
    DataBufferPointer m_data;

public:
    SignalNode_SineGenerator(SignalGraph* sg);

    ~SignalNode_SineGenerator();

private:
    virtual void build() override final;
};


class SignalNode_ScalarExtractor : public SignalNode{
    R64FX_NODE_SINK(in)
    R64FX_NODE_SOURCE(out)

public:
    SignalNode_ScalarExtractor(SignalGraph* sg);

    ~SignalNode_ScalarExtractor();

private:
    virtual void build() override final;
};

}//namespace

#endif//R64FX_SIGNAL_NODE_SINE_GENERATOR_HPP
