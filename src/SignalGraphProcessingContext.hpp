#ifndef R64FX_SIGNAL_GRAPH_PROCESSING_CONTEXT_HPP
#define R64FX_SIGNAL_GRAPH_PROCESSING_CONTEXT_HPP

#include "jit.hpp"

namespace r64fx{

class SignalGraphProcessingContext{
    Assembler* m_assebler = nullptr;

public:
    SignalGraphProcessingContext(Assembler* assebler) : m_assebler(assebler) {}

    inline Assembler* assembler() const { return m_assebler; }
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_PROCESSING_CONTEXT_HPP
