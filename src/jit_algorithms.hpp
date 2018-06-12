#ifndef R64FX_JIT_ALGORITHMS_HPP
#define R64FX_JIT_ALGORITHMS_HPP

#include "jit.hpp"

namespace r64fx{

class ProcedureList{};

constexpr float  INT_RCP     = 1.0f / int(0x7FFFFFFF);
constexpr float  PI_INT_RCP  = INT_RCP * 3.1415926535897932384626f;
constexpr unsigned long fact(unsigned long n) { if(n == 0) return 1; return n * fact(n - 1); }

struct JitAlgorithmConstants{
    struct{
        float pi_int_rcp  = PI_INT_RCP;
        float one         = 1.0f;
        float f2_rcp      = 1.0f / fact(2);
        float f4_rcp      = 1.0f / fact(4);
    }v0;

    struct{
        float f6_rcp      = 1.0f / fact(6);
        float f8_rcp      = 1.0f / fact(8);
        float f10_rcp     = 1.0f / fact(10);
        float f           = 0.0f;
    }v1;
};

struct SineOscBuffers{
    struct{
        int osc[4] = {0, 0, 0, 0};
    }v0;

    struct{
        int increment[4] = {0, 0, 0, 0};
    }v1;
};

template<typename AssemblerT> inline void gen_sine_osc(AssemblerT* as,
    GPR64 constants,  //Pointer to JitAlgorithmConstants
    GPR64 buffers,    //Pointer to SineOscBuffers
    Xmm v,            //Resusting Value
    Xmm c,            //Constants
    Xmm f,            //Temporary
    Xmm s,            //X Squared
    Xmm x             //Running X Value
)
{
    as->MOVAPS   (c, Base(constants));

    /* Osc Clock */
    as->MOVAPS   (f, Base(buffers));

    as->CVTDQ2PS (s, f); //Use osc value before incrementing.
    as->PADDD    (f, Base(buffers) + Disp(sizeof(float) * 4));
    as->MOVAPS   (Base(buffers), f);

    as->PSHUFD   (f, c, Shuf(0, 0, 0, 0));
    as->MULPS    (s, f);

    /* Cosine Wave */
    as->MULPS  (s, s);
    as->MOVAPS (x, s);

    // 1
    as->PSHUFD (v, c, Shuf(1, 1, 1, 1));

    // X^2
    as->PSHUFD (f, c, Shuf(2, 2, 2, 2));
    as->MULPS  (f, x);
    as->SUBPS  (v, f);

    // X^4
    as->MULPS  (x, s);
    as->PSHUFD (f, c, Shuf(3, 3, 3, 3));
    as->MULPS  (f, x);
    as->ADDPS  (v, f);

    as->MOVAPS (c, Base(constants) + Disp(sizeof(float) * 4));

    // X^6
    as->MULPS  (x, s);
    as->PSHUFD (f, c, Shuf(0, 0, 0, 0));
    as->MULPS  (f, x);
    as->SUBPS  (v, f);

    //X^8
    as->MULPS  (x, s);
    as->PSHUFD (f, c, Shuf(1, 1, 1, 1));
    as->MULPS  (f, x);
    as->ADDPS  (v, f);

    // X^10
    as->MULPS  (x, s);
    as->PSHUFD (f, c, Shuf(2, 2, 2, 2));
    as->MULPS  (f, x);
    as->SUBPS  (v, f);
}




template<typename AssemblerT> inline void gen_loop_player(AssemblerT* as,
    GPR64 playhead,
    GPR64 sound,
    Xmm p,
    Xmm m
)
{
    as->MOVAPS(p, Base(playhead));
    as->PADDQ(p, Base(playhead) + Disp(16));
}


/* Interpolation of float data from 4 mono buffers. */
template<typename BaseT> struct JitAlgorithm_Playback : public BaseT{
    R64FX_USING_JIT_METHODS(BaseT)

public:
    struct Constants{
        float int2float = 1.0f / float(0x7FFFFFFF);
        int   sign      = 0x80000000;
        float one       = 1.0f;
        int   all_bits  = -1;
    };

    /* Playhead position as fixed point p:p . */
    struct Playhead{
        int msb[4] = {};
        int lsb[4] = {};
    };


    inline void build(float* output, Constants* constants, float* data, Playhead* playhead, int count = 1)
    {
        R64FX_DEBUG_ASSERT(constants);
        R64FX_DEBUG_ASSERT(data);
        R64FX_DEBUG_ASSERT(playhead);
        R64FX_DEBUG_ASSERT(count >= 1);

        Xmm pp      = xmm0;
        Xmm r0      = xmm1;
        Xmm r1      = xmm2;
        Xmm r2      = xmm3;
        Xmm r3      = xmm4;
        Xmm const0  = xmm5;
        Xmm const1  = xmm6;

        GPR64 d = rax; //Data Base Pointer
        GPR64 c = rcx; //Loop Counter
        GPR64 p = rdx; //Playhead Base Pointer
        GPR64 x = rbx; //Data Fetch Index

        MOV      (d, ImmAddr(constants));
        MOVAPS   (const0, Base(d));

        MOV      (d, ImmAddr(data));
        MOV      (p, ImmAddr(playhead));
        MOV      (c, Imm32(sizeof(Playhead) * -count));

        JumpLabel loop;
        mark(loop);
        {
            /* Fetch Playhead MSB */
            MOVAPS   (pp, Base(p) + Index(c));

            /* Fetch Chan0 */
            MOVD     (x,  pp);
            MOVLPS   (r0, Base(d) + Index(x) * 4);
            PSHUFD   (pp, pp, Shuf(1, 2, 3, 0));

            /* Fetch Chan1 */
            MOVD     (x,  pp);
            MOVHPS   (r0, Base(d) + Index(x) * 4);
            PSHUFD   (pp, pp, Shuf(1, 2, 3, 0));

            /* Fetch Chan2 */
            MOVD     (x,  pp);
            MOVLPS   (r1, Base(d) + Index(x) * 4);
            PSHUFD   (pp, pp, Shuf(1, 2, 3, 0));

            /* Fetch Chan3 */
            MOVD     (x, pp);
            MOVHPS   (r1, Base(d) + Index(x) * 4);
            PSHUFD   (pp, pp, Shuf(1, 2, 3, 0));


            /* Fetch Playhead LSB */
            MOVAPS   (pp, Base(p) + Index(c) + Disp(16));

            /* Shuffle even numbered scalars into rr0 and odd numbered scalars into rr2. */
            MOVAPS   (r2, r0);
            SHUFPS   (r0, r1, Shuf(0, 2, 0, 2));
            SHUFPS   (r2, r1, Shuf(1, 3, 1, 3));

            /* Convert LSB to float. */
            PSRLD    (pp, Imm8(1));
            CVTDQ2PS (pp, pp);
            PSHUFD   (const1, const0, Shuf(0, 0, 0, 0));
            MULPS    (pp, const1);

            /* Prepare second coeff. */
            MOVAPS   (r3, pp);
            PSHUFD   (const1, const0, Shuf(1, 1, 1, 1));
            ORPS     (pp, const1);
            PSHUFD   (const1, const0, Shuf(2, 2, 2, 2));
            ADDPS    (pp, const1);

            /* Perform interpolation. */
            MULPS    (pp, r0);
            MULPS    (r3, r2);
            ADDPS    (pp, r3);


            ADD      (c, Imm8(sizeof(Playhead)));
            JNZ      (loop);
        }
    }
};

}//namespace

#endif//R64FX_JIT_ALGORITHMS_HPP
