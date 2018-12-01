#ifndef R64FX_JIT_PROCEDURES_HPP
#define R64FX_JIT_PROCEDURES_HPP

#include "jit.hpp"

namespace r64fx{

class JitProcSequence;

/* Collection of jit procedures. */
class JitProcedures : Assembler{
    int m_exit_offset = 0;

public:
    JitProcedures()
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

        XOR  (rax, rax);

        R64FX_JIT_LABEL(main_loop); //Iterate over nframes
        MOV  (rdx, Base(rdi));      //Load first proc addr
        XOR  (rbp, rbp);            //Reset JitProcSequence index
        JMP  (rdx);                 //Jump to first proc

        /* ------------------------------------------- */

        m_exit_offset = Assembler::bytesUsed();
        ADD  (rsi, Imm8(1));
        JNZ  (main_loop);

        POP  (r12);
        POP  (rsi);
        POP  (rdi);
        POP  (rbp);
        POP  (rbx);
        RET  ();

        genProc_Gain();
        genProc_Mix();
        permitExecution();
    }

    /* Don't forget to add this to the end of the sequence! */
    class Exit; inline Exit* procExit() const { return (Exit*)(Assembler::begin() + m_exit_offset); }

    inline static void CheckType(Exit*) {};

private:
    /* Close loop with JMP(ptr). */
    inline void loop(GPR64 ptr, GPR64 cnt, long n, GPR64 nxt)
    {
        leaNextInstruction(ptr);
        ADD   (cnt, Imm8(n));
        CMOVZ (ptr, nxt);
    }

    /* Unpack pointers stored in JitProcSequence into GPRs. */
    template<class... GPRS> inline void unpack(GPRS... gprs)
    {
        unpack_(8, gprs...);
        ADD(rbp, Imm32(sizeof...(gprs) + 1));
    }

    template<class GPR, class... GPRS> inline void unpack_(int n, GPR gpr, GPRS... gprs)
    {
        MOV(gpr, Base(rdi) + Index(rbp)*8 + Disp(n));
        if constexpr(sizeof...(gprs) > 0) unpack_(n+8, gprs...);
    }

#define DEF_JIT_PROC(PROCNAME, ...)                                             \
public:                                                                         \
    class PROCNAME; inline PROCNAME* proc##PROCNAME() const                     \
    {                                                                           \
        return (PROCNAME*)(Assembler::begin() + m_##PROCNAME##_offset);         \
    }                                                                           \
                                                                                \
    inline static void CheckType(PROCNAME*, __VA_ARGS__) {}                     \
                                                                                \
private:                                                                        \
    int m_##PROCNAME##_offset;                                                  \
                                                                                \
    inline void genProc_##PROCNAME() /* Save procedure start offset */          \
    {                                                                           \
        m_##PROCNAME##_offset = Assembler::bytesUsed();                         \
        genProc##PROCNAME();                                                    \
    }                                                                           \
                                                                                \
    inline void genProc##PROCNAME()
        /* Follow with function body. */

    DEF_JIT_PROC(Gain, float*, float*, float*, long)
    {
        unpack(r8, r9, r10, rcx, rbx);
        loop(rdx, rcx, 4, rbx);
            ADD   (rax, Imm8(1));
            MOVAPS (xmm0, Base(r8) + Index(rcx)*4);
            MULPS  (xmm0, Base(r9) + Index(rcx)*4);
            MOVAPS (Base(r10) + Index(rcx)*4, xmm0);
        JMP(rdx);
    }

    DEF_JIT_PROC(Mix, float*, long)
    {
        unpack(r8, rcx, rbx);
        loop(rdx, rcx, 4, rbx);
            
        JMP(rdx);
    }

#undef DEF_JIT_PROC
};

/* Make sure to use the same JitProcedures instances for add() and exec() calls! */
class JitProcSequence{
    MemoryBuffer m_buffer;

    template<typename Arg, typename... Args> inline void pack(Arg arg, Args... args)
    {
        *((long*)m_buffer.grow(sizeof(long))) = (long)arg;
        if constexpr(sizeof...(args) > 0) pack(args...);
    }

public:
    template<typename... Args> inline auto &add(Args... args)
    {
        JitProcedures::CheckType(args...); //Make sure that compilation fails if wrong arguments are given!
        pack(args...);
        return *this;
    }

    /* Execute sequence. */
    inline long exec(JitProcedures* jp, long nframes)
    {
        R64FX_DEBUG_ASSERT(jp);
        R64FX_DEBUG_ASSERT(nframes > 0);

        auto proc = (long(*)(void*, long)) ((Assembler*)jp)->begin();
        return proc(m_buffer.begin(), -nframes);
    }

    inline void clear() { m_buffer.resize(0); }
};

}//namespace r64fx

// 
// 
// R64FX_DEF_JIT_PROC_PARAMS(Gain, float* src, float* coeffs, float* dst, long count)
// {
//     R64FX_DEBUG_ASSERT(src);
//     R64FX_DEBUG_ASSERT(coeffs);
//     R64FX_DEBUG_ASSERT(dst);
//     R64FX_DEBUG_ASSERT(count > 0);
//     R64FX_DEBUG_ASSERT((count & 3) == 0);
//     R64FX_SAVE_JIT_PROC_Args(src, coeffs, dst, -count);
// }
// 
// R64FX_DEF_JIT_PROC(Gain)
// {
//     R64FX_LOAD_JIT_PROC_Args(r8, r9, r10, rcx);
//     R64FX_JIT_PROC_LOOP(rdx, rcx, 4);
//         MOVAPS (xmm0, Base(r8) + Index(rcx)*4);
//         MULPS  (xmm0, Base(r9) + Index(rcx)*4);
//         MOVAPS (Base(r10) + Index(rcx)*4, xmm0);
//     JMP(rdx); //Next Iteration or Next Proc.
// }
// 
// R64FX_DEF_JIT_PROC_PARAMS(Playback, int* playheads, float* buffers, float* dst, long count)
// {
//     R64FX_DEBUG_ASSERT(playheads);
//     R64FX_DEBUG_ASSERT(buffers);
//     R64FX_DEBUG_ASSERT(count > 0);
//     R64FX_DEBUG_ASSERT((count & 3) == 0);
//     R64FX_SAVE_JIT_PROC_Args(playheads+(count<<2)-16, buffers, dst+count-4, -(count<<2));
// }
// 
// R64FX_DEF_JIT_PROC(Playback)
// {
//     R64FX_LOAD_JIT_PROC_Args(r8, r9, r10, rcx);
//     R64FX_JIT_PROC_LOOP(rdx, rcx, 16);
// 
//         /* Fetch Playhead */
//         MOVDQA (xmm0, Base(r8) + Index(rcx)*4);
// 
//         /* Fetch Data */
//         XORPS  (xmm1, xmm1);
// 
//         MOV(r11, Imm32(-4));
//         R64FX_JIT_LABEL(FetchData);
//             MOVD   (rax, xmm0);
//             SHUFPS (xmm0, xmm0, Shuf(1, 2, 3, 0));
//             MOVSS  (xmm2, Base(r9) + Index(rax)*4);
//             ORPS   (xmm1, xmm2);
//             SHUFPS (xmm1, xmm1, Shuf(1, 2, 3, 0));
//         ADD(r11, Imm8(1));
//         JNZ(FetchData);
// 
//         /* Store Output */
//         MOVAPS  (Base(r10) + Index(rcx), xmm1);
// 
//         /* Move Playhead */
//         PADDD   (xmm0, Base(r8) + Index(rcx)*4 + Disp(16));
// 
//         /* Loop Playhead */
//         MOVDQA  (xmm1, xmm0);
//         MOVDQA  (xmm2, Base(r8) + Index(rcx)*4 + Disp(32));
//         PCMPGTD (xmm1, xmm2);
//         MOVDQA  (xmm3, xmm1);
//         PAND    (xmm1, xmm2);
//         PAND    (xmm3, Base(r8) + Index(rcx)*4 + Disp(48));
//         PSUBD   (xmm0, xmm1);
//         PADDD   (xmm0, xmm3);
// 
//         /* Store Playhead */
//         MOVDQA (Base(r8) + Index(rcx)*4, xmm0);
//     JMP(rdx);
// }

// }//namespace r64fx

#endif//R64FX_JIT_PROCEDURES_HPP
