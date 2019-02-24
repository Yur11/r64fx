#include "jit_procedures.hpp"

namespace r64fx{

JitProc::JitProc()
{
    PUSH (rbx);
    PUSH (rbp);
    PUSH (rdi);
    PUSH (rsi);
    PUSH (r12);

    /* Begin Time */
    LFENCE ();
    RDTSC  ();
    SHL    (rdx, Imm8(32));
    OR     (rdx, rax);
    PUSH   (rdx);

    R64FX_JIT_LABEL(main_loop); //Iterate over nframes passed via rsi
    MOV  (rdx, Base(rdi));      //Load first proc addr
    XOR  (rbp, rbp);            //Reset JitProcSequence index
    JMP  (rdx);                 //Jump to first proc

    /* ------------------------------------------- */

    m_offset.Exit = bytesUsed();
    ADD  (rsi, Imm8(1));
    JNZ  (main_loop);

    /* End Time */
    LFENCE ();
    RDTSC  ();
    SHL    (rdx, Imm8(32));
    OR     (rax, rdx);
    POP    (rdx);
    SUB    (rax, rdx);

    POP  (r12);
    POP  (rsi);
    POP  (rdi);
    POP  (rbp);
    POP  (rbx);
    RET  ();

    genProc_AddF4();
    genProc_MulF4();
    genProc_OscTaylorCosF4();
    genProc_OscTaylorCosF8();
    permitExecution();
}


void JitProc::genProc_AddF4()
{
    m_offset.AddF4 = bytesUsed();

    unpack(rax, rcx, rdx, rbx, r8);
    loop(rax, 16, r9, r8);
        MOVAPS (xmm0, Base(rcx) + Index(rax));
        ADDPS  (xmm0, Base(rdx) + Index(rax));
        MOVAPS (Base(rbx) + Index(rax), xmm0);
    JMP(r9);
}


void JitProc::genProc_MulF4()
{
    m_offset.MulF4 = bytesUsed();

    unpack(rax, rcx, rdx, rbx, r8);
    loop(rax, 16, r9, r8);
        MOVAPS (xmm0, Base(rcx) + Index(rax));
        MULPS  (xmm0, Base(rdx) + Index(rax));
        MOVAPS (Base(rbx) + Index(rax), xmm0);
    JMP(r9);
}


void JitProc::genProc_OscTaylorCosF4()
{
    m_offset.OscTaylorCosF4 = bytesUsed();

    unpack(rax, rcx, rdx, rbx, r8, r9);
    loop(rax, 16, r10, r9);
        /* OSC clock */
        MOVAPS   (xmm1, Base(rcx) + Index(rax));
        CVTDQ2PS (xmm0, xmm1);
        PADDD    (xmm1, Base(rdx) + Index(rax));
        MOVAPS   (Base(rcx) + Index(rax), xmm1);

        /* x: (-Pi, Pi) */
        MULPS    (xmm0, Base(r8));

        /* 1 */
        MOVAPS   (xmm1, Base(r8) + Disp(16));

        /* x^2 */
        MULPS    (xmm0, xmm0);
        MOVAPS   (xmm2, xmm0);

        /* - x^2 / 2! */
        MULPS    (xmm2, Base(r8) + Disp(32));
        SUBPS    (xmm1, xmm2);

        /* x^4 */
        MOVAPS   (xmm2, xmm0);
        MULPS    (xmm2, xmm0);
        MOVAPS   (xmm3, xmm2);

        /* + x^4 / 4! */
        MULPS    (xmm2, Base(r8) + Disp(48));
        ADDPS    (xmm1, xmm2);

        /* x^6 */
        MOVAPS   (xmm2, xmm3);
        MULPS    (xmm2, xmm0);
        MOVAPS   (xmm3, xmm2);

        /* - x^6 / 6!*/
        MULPS    (xmm2, Base(r8) + Disp(64));
        SUBPS    (xmm1, xmm2);

        /* x^8 */
        MULPS    (xmm3, xmm0);

        /* + x^8 / 8! */
        MULPS    (xmm3, Base(r8) + Disp(80));
        ADDPS    (xmm1, xmm3);

        /* Out */
        MOVAPS   (Base(rbx) + Index(rax), xmm1);
    JMP(r10);
}


void JitProc::genProc_OscTaylorCosF8()
{
    m_offset.OscTaylorCosF8 = bytesUsed();

    unpack(rax, rcx, rdx, rbx, r8, r9);
    loop(rax, 32, r10, r9);
        /* OSC clock */
        MOVAPS   (xmm1, Base(rcx) + Index(rax));
        MOVAPS   (xmm5, Base(rcx) + Index(rax) + Disp(16));
        CVTDQ2PS (xmm0, xmm1);
        CVTDQ2PS (xmm4, xmm5);
        PADDD    (xmm1, Base(rdx) + Index(rax));
        PADDD    (xmm5, Base(rdx) + Index(rax) + Disp(16));
        MOVAPS   (Base(rcx) + Index(rax),            xmm1);
        MOVAPS   (Base(rcx) + Index(rax) + Disp(16), xmm5);

        /* x: (-Pi, Pi) */
        MULPS    (xmm0, Base(r8));
        MULPS    (xmm4, Base(r8));

        /* 1 */
        MOVAPS   (xmm1, Base(r8) + Disp(16));
        MULPS    (xmm5, Base(r8));

        /* x^2 */
        MULPS    (xmm0, xmm0);
        MULPS    (xmm4, xmm4);
        MOVAPS   (xmm2, xmm0);
        MOVAPS   (xmm4, xmm4);

        /* - x^2 / 2! */
        MULPS    (xmm2, Base(r8) + Disp(32));
        MULPS    (xmm6, Base(r8) + Disp(32));
        SUBPS    (xmm1, xmm2);
        SUBPS    (xmm5, xmm6);

        /* x^4 */
        MOVAPS   (xmm2, xmm0);
        MOVAPS   (xmm6, xmm4);
        MULPS    (xmm2, xmm0);
        MULPS    (xmm6, xmm4);
        MOVAPS   (xmm3, xmm2);
        MOVAPS   (xmm7, xmm6);

        /* + x^4 / 4! */
        MULPS    (xmm2, Base(r8) + Disp(48));
        MULPS    (xmm6, Base(r8) + Disp(48));
        ADDPS    (xmm1, xmm2);
        ADDPS    (xmm5, xmm6);

        /* x^6 */
        MOVAPS   (xmm2, xmm3);
        MOVAPS   (xmm6, xmm7);
        MULPS    (xmm2, xmm0);
        MULPS    (xmm6, xmm4);
        MOVAPS   (xmm3, xmm2);
        MOVAPS   (xmm7, xmm6);

        /* - x^6 / 6!*/
        MULPS    (xmm2, Base(r8) + Disp(64));
        MULPS    (xmm6, Base(r8) + Disp(64));
        SUBPS    (xmm1, xmm2);
        SUBPS    (xmm5, xmm6);

        /* x^8 */
        MULPS    (xmm3, xmm0);
        MULPS    (xmm7, xmm4);

        /* + x^8 / 8! */
        MULPS    (xmm3, Base(r8) + Disp(80));
        MULPS    (xmm7, Base(r8) + Disp(80));
        ADDPS    (xmm1, xmm3);
        ADDPS    (xmm5, xmm7);

        /* Out */
        MOVAPS   (Base(rbx) + Index(rax),            xmm1);
        MOVAPS   (Base(rbx) + Index(rax) + Disp(16), xmm5);
    JMP(r10);
}


void JitProc::loop(GPR64 counter, long increment, GPR64 jump_ptr, GPR64 jump_next)
{
    leaNextInstruction(jump_ptr);
    ADD(counter, Imm8(increment));
    CMOVZ(jump_ptr, jump_next);
}

}//namespace r64fx
