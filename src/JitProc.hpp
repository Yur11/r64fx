#ifndef R64FX_JIT_PROC_HPP
#define R64FX_JIT_PROC_HPP

#include "Jit.hpp"

namespace r64fx{

class JitDspLoop : public Assembler{
public:
    class LoopEntryPtr;

    inline LoopEntryPtr* prologue()
    {
        PUSH (rbx);
        PUSH (rbp);
        PUSH (rdi);
        PUSH (rsi);
        PUSH (r12);

        /* Iterate over nframes passed through rdi using */
        auto loop_entry = (LoopEntryPtr*) MemoryBuffer::ptr();

        return loop_entry;
    }

    inline void epilogue(LoopEntryPtr* loop_entry)
    {
        ADD  (rsi, Imm8(1));
        JNZ  (Rel((unsigned char*)loop_entry));

        POP  (r12);
        POP  (rsi);
        POP  (rdi);
        POP  (rbp);
        POP  (rbx);
        RET  ();
    }
};

}//namespace r64fx

#endif//R64FX_JIT_PROC_HPP