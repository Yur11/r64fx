#ifndef R64FX_JIT_PROCEDURES_HPP
#define R64FX_JIT_PROCEDURES_HPP

#include "jit.hpp"

/* === Wrap some boilerplate code with macros === */

/* Define function to load parameters into proc sequence. */
#define R64FX_DEF_JIT_PROC_PARAMS(NAME, ...)                                    \
    class JitProcAddr_##NAME;                                                   \
                                                                                \
    void add(JitProcSequence &seq, JitProcAddr_##NAME* proc_addr, __VA_ARGS__)
    //Follow with function body!

#define R64FX_SAVE_JIT_PROC_ARGS(...)                                           \
    seq.saveJitProcArgs(proc_addr, __VA_ARGS__)

/* Define jit procedure. */
#define R64FX_DEF_JIT_PROC(NAME)                                                \
    class JitProcAddr_##NAME;                                                   \
                                                                                \
    template<class Parent> class BaseJitProc_##NAME                             \
    : public Parent{                                                            \
        int m_offset = 0;                                                       \
    protected:                                                                  \
        BaseJitProc_##NAME() { m_offset = Assembler::bytesUsed(); }             \
    public:                                                                     \
        inline auto proc_##NAME() const                                         \
            { return (JitProcAddr_##NAME*)(Assembler::begin() + m_offset); }    \
    };                                                                          \
                                                                                \
    template<class Parent> class JitProc_##NAME                                 \
    : public BaseJitProc_##NAME<Parent>{                                        \
        R64FX_USING_JIT_METHODS(Assembler)                                      \
    public:                                                                     \
        JitProc_##NAME();                                                       \
    };                                                                          \
                                                                                \
    template<class Parent> JitProc_##NAME<Parent>::JitProc_##NAME()
    //Follow with function body!

/* Load proc args into registers. Load next proc addr into rbx. */
#define R64FX_LOAD_JIT_PROC_ARGS(...)                                           \
    JitProcCommon::loadJitProcArgs(__VA_ARGS__, rbx)

/* Load loop entry addr into PTR.
 * Increment CNT. It is negative and grows towards zero.
 * Load next proc addr into PTR if CNT reached zero.
 */
#define R64FX_JIT_PROC_LOOP(PTR, CNT, N)                                        \
    leaNextInstruction(PTR);                                                    \
    ADD    (CNT, Imm8(N));                                                      \
    CMOVZ  (PTR, rbx)

#define R64FX_JIT_LABEL(NAME) JumpLabel NAME; mark(NAME)

namespace r64fx{

class JitProcSequence{
    MemoryBuffer m_buffer;

public:
    inline void store(long val)
    {
        *((long*)m_buffer.grow(sizeof(long))) = val;
    }

    inline void* start() const { return m_buffer.begin(); }

    inline void clear() { m_buffer.resize(0); }

    template<typename... Args> void saveJitProcArgs(Args... args) { saveNextArg<Args...>(args...); }

private:
    template<typename Arg, typename... Args> void saveNextArg(Arg arg, Args... args)
    {
        store((long)arg); if constexpr(sizeof...(args) > 0) saveNextArg<Args...>(args...);
    }
};


class JitProcCommon : public Assembler{
    int m_exit_offset = 0;

public:
    JitProcCommon()
    {
        /* Using Base(rdi) + Index(rbp)*8 to access JitProcSequence,
         * where rdi is obtained as first argument in System V AMD64 ABI calling convention and stays unchanged,
         * rbp is incremented by each procedure.
         *
         * Using rsi as main buffer index.
         * It is negative and is incremented towards zero. */

        PUSH (rbx);
        PUSH (rbp);
        PUSH (rdi);
        PUSH (rsi);
        PUSH (r12);

        XOR(rax, rax);

        JumpLabel main_loop_entry; mark(main_loop_entry);
        MOV  (rdx, Base(rdi));  //Load first proc addr.
        XOR  (rbp, rbp);        //Reset JitProcSequence index
        JMP  (rdx);             //Jump to first proc.

        /* ------------------------------------------- */

        m_exit_offset = Assembler::bytesUsed();
        ADD  (rsi, Imm8(1));
        JNZ  (main_loop_entry);

        POP  (r12);
        POP  (rsi);
        POP  (rdi);
        POP  (rbp);
        POP  (rbx);
        RET  ();

        permitExecution();
    }

    inline long exec(JitProcSequence &jps, long nframes)
    {
        R64FX_DEBUG_ASSERT(nframes > 0);

        auto proc = (long(*)(void*, long)) Assembler::begin();
        return proc(jps.start(), -nframes);
    }

    inline void* procSeqExit() const { return Assembler::begin() + m_exit_offset; }

protected:
    template<typename... GPRS> inline void loadJitProcArgs(GPRS... gprs)
    {
        loadNextArg<1, GPRS...>(gprs...);
        ADD(rbp, Imm8(sizeof...(GPRS)));
    }

private:
    template<int N, typename GPR, typename... GPRS> inline void loadNextArg(GPR gpr, GPRS... gprs)
    {
        MOV(gpr, Base(rdi) + Index(rbp)*8 + Disp(N*8));
        if constexpr(sizeof...(gprs) > 0)
            loadNextArg<N+1, GPRS...>(gprs...);
    }
};


R64FX_DEF_JIT_PROC_PARAMS(Gain, float* src, float* coeffs, float* dst, long count)
{
    R64FX_DEBUG_ASSERT(src);
    R64FX_DEBUG_ASSERT(coeffs);
    R64FX_DEBUG_ASSERT(dst);
    R64FX_DEBUG_ASSERT(count > 0);
    R64FX_DEBUG_ASSERT((count & 3) == 0);
    R64FX_SAVE_JIT_PROC_ARGS(src, coeffs, dst, -count);
}

R64FX_DEF_JIT_PROC(Gain)
{
    R64FX_LOAD_JIT_PROC_ARGS(r8, r9, r10, rcx);
    R64FX_JIT_PROC_LOOP(rdx, rcx, 4);
        MOVAPS (xmm0, Base(r8) + Index(rcx)*4);
        MULPS  (xmm0, Base(r9) + Index(rcx)*4);
        MOVAPS (Base(r10) + Index(rcx)*4, xmm0);
    JMP(rdx); //Next Iteration or Next Proc.
}

R64FX_DEF_JIT_PROC_PARAMS(Playback, int* playheads, float* buffers, float* dst, long count)
{
    R64FX_DEBUG_ASSERT(playheads);
    R64FX_DEBUG_ASSERT(buffers);
    R64FX_DEBUG_ASSERT(count > 0);
    R64FX_DEBUG_ASSERT((count & 3) == 0);
    R64FX_SAVE_JIT_PROC_ARGS(playheads+(count<<2)-16, buffers, dst+count-4, -(count<<2));
}

R64FX_DEF_JIT_PROC(Playback)
{
    R64FX_LOAD_JIT_PROC_ARGS(r8, r9, r10, rcx);
    R64FX_JIT_PROC_LOOP(rdx, rcx, 16);

        /* Fetch Playhead */
        MOVDQA (xmm0, Base(r8) + Index(rcx)*4);

        /* Fetch Data */
        XORPS  (xmm1, xmm1);

        MOV(r11, Imm32(-4));
        R64FX_JIT_LABEL(FetchData);
            MOVD   (rax, xmm0);
            SHUFPS (xmm0, xmm0, Shuf(1, 2, 3, 0));
            MOVSS  (xmm2, Base(r9) + Index(rax)*4);
            ORPS   (xmm1, xmm2);
            SHUFPS (xmm1, xmm1, Shuf(1, 2, 3, 0));
        ADD(r11, Imm8(1));
        JNZ(FetchData);

        /* Store Output */
        MOVAPS  (Base(r10) + Index(rcx), xmm1);

        /* Move Playhead */
        PADDD   (xmm0, Base(r8) + Index(rcx)*4 + Disp(16));

        /* Loop Playhead */
        MOVDQA  (xmm1, xmm0);
        MOVDQA  (xmm2, Base(r8) + Index(rcx)*4 + Disp(32));
        PCMPGTD (xmm1, xmm2);
        MOVDQA  (xmm3, xmm1);
        PAND    (xmm1, xmm2);
        PAND    (xmm3, Base(r8) + Index(rcx)*4 + Disp(48));
        PSUBD   (xmm0, xmm1);
        PADDD   (xmm0, xmm3);

        /* Store Playhead */
        MOVDQA (Base(r8) + Index(rcx)*4, xmm0);
    JMP(rdx);
}

}//namespace r64fx

#endif//R64FX_JIT_PROCEDURES_HPP
