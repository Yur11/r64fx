#ifndef R64FX_SIGNAL_NODE_OSCILLATOR_HPP
#define R64FX_SIGNAL_NODE_OSCILLATOR_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_OscClock : public SignalNode{
    R64FX_NODE_SOURCE(out);

    int m_delta = 0;

public:
    SignalNode_OscClock(SignalGraph* sg);

    inline void setDelta(int delta) { m_delta = delta; }

    inline int delta() const { return m_delta; }

private:
    virtual void build() override final;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_OSCILLATOR_HPP
