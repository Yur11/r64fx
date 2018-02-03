#ifndef R64FX_JIT_ALGORITHMS_HPP
#define R64FX_JIT_ALGORITHMS_HPP

#include "jit.hpp"

namespace r64fx{

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

}//namespace

#endif//R64FX_JIT_ALGORITHMS_HPP
