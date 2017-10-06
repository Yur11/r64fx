#include "SignalNode_BufferRW.hpp"

namespace r64fx{

void SignalNode_BufferReader::build(SignalGraphProcessor &sgp)
{
    auto &as = sgp.assembler();
    auto s = source().port();
}

void SignalNode_BufferWriter::build(SignalGraphProcessor &sgp)
{
    auto &as = sgp.assembler();
    auto s = sink().port();
}

}//namespace r64fx
