#include "SignalNode_BufferRW.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

void SignalNode_BufferReader::build(SignalGraphProcessor &sgp)
{
//     auto s = source().port();
//     sgp.memoryStorage(s->storage(), 1, 1);
// 
//     auto &as = sgp.assembler();
//     as.mov(rsi, ImmAddr(buffer() + sgp.mainBufferSize()));
//     as.mov(rax, Base(rsi) + Index(rcx, 4));
// 
//     SignalDataStorage_Memory mem_storage(s->storage()[0]);
//     as.mov(Base(rdi) + Disp(mem_storage.index() * 4), rax);
    cout << "reader: " << this << "\n";
}

void SignalNode_BufferWriter::build(SignalGraphProcessor &sgp)
{
//     auto source = sink().port()->connectedSource();
//     if(!source)
//         return;
// 
//     R64FX_DEBUG_ASSERT(source->storage()->isMemoryStorage());
//     SignalDataStorage_Memory mem_storage(source->storage()[0]);
// 
//     auto &as = sgp.assembler();
//     as.mov(rax, Base(rdi) + Disp(mem_storage.index() * 4));
// 
//     as.mov(rsi, ImmAddr(buffer() + sgp.mainBufferSize()));
//     as.mov(Base(rsi) + Index(rcx, 4), rax);
    cout << "writer: " << this << "\n";
}

}//namespace r64fx
