#include <iostream>
#include "jit.hpp"

using namespace std;
using namespace r64fx;

class MyAssembler : public Assembler{
public:
    MyAssembler()
    {
        resize(1);
        permitExecution();

        /* Fetch Clocks */
        MOVDQA(xmm0, Base(rdi) + Disp(32));
        MOVDQA(xmm1, xmm0);

        /* Mask Low and High bits */
        PAND(xmm0, Base(rdi));
        PAND(xmm1, Base(rdi) + Disp(16));

        /* Fetch frames */
        PXOR(xmm2, xmm2);
        PEXTRW(rax, xmm1, Imm8(1));
        MOVD(xmm2, Base(rdi) + Index(rax)*2 + Disp(48));

        PXOR(xmm3, xmm3);
        PEXTRW(rax, xmm1, Imm8(3));
        MOVD(xmm3, Base(rdi) + Index(rax)*2 + Disp(48));
        PSHUFD(xmm2, xmm2, Shuf(1, 2, 3, 0));
        POR(xmm2, xmm3);

        PXOR(xmm3, xmm3);
        PEXTRW(rax, xmm1, Imm8(5));
        MOVD(xmm3, Base(rdi) + Index(rax)*2 + Disp(48));
        PSHUFD(xmm2, xmm2, Shuf(1, 2, 3, 0));
        POR(xmm2, xmm3);

        PXOR(xmm3, xmm3);
        PEXTRW(rax, xmm1, Imm8(7));
        MOVD(xmm3, Base(rdi) + Index(rax)*2 + Disp(48));
        PSHUFD(xmm2, xmm2, Shuf(1, 2, 3, 0));
        POR(xmm2, xmm3);

        PSHUFD(xmm2, xmm2, Shuf(1, 2, 3, 0));

        /* Calculate coeffs */
        MOVDQA(xmm1, xmm0);
        PSLLD(xmm1, Imm8(16));
        POR(xmm0, xmm1);

        /* Out */
        MOVDQA(Base(rsi), xmm0);

        RET();
    }

    long run(void* d, void* s)
    {
        return ((long (*)(void*, void*)) begin())(d, s);
    }
};

unsigned short wt[] = {
    0xFFFF, 0x0000, 0xFFFF, 0x0000,  0xFFFF, 0x0000, 0xFFFF, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0xFFFF,  0x0000, 0xFFFF, 0x0000, 0xFFFF,

    /* Clocks */
    0xAAAA, 0x0000, 0xBBBB, 0x0001,  0xCCCC, 0x0002, 0xDDDD, 0x0003,

    0xFF01, 0xFF02, 0xFF03, 0xFF04,  0xFF05, 0xFF06, 0xFF07, 0xFF07,
    0x0F01, 0x0000, 0x0000, 0x0000,
};

unsigned short out[] = {
    0x0000, 0x0000, 0x0000, 0x0000,  0x0000, 0x0000, 0x0000, 0x0000
};

int main()
{
    MyAssembler assembler;
    cout << hex << assembler.run(wt, out) << "\n";

    for(int i=0; i<8; i++)
        cout << hex << out[i] << (i<7 ? ", " : "\n");

    return 0;
}
