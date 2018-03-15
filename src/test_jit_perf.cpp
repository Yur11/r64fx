#include "test.hpp"
#include "jit.hpp"

using namespace std;
using namespace r64fx;


struct TestJitPerf : public Assembler{

TestJitPerf() { resize(1, 1); }

void initRegs()
{
    MOV     (rax, Imm64ff(1.0f, 1.0f));
    MOVQ    (xmm0, rax);
    MOVLHPS (xmm0, xmm0);
    MOVAPS  (xmm1, xmm0);
    MOVAPS  (xmm2, xmm0);
    MOVAPS  (xmm3, xmm0);
    MOVAPS  (xmm4, xmm0);
    MOVAPS  (xmm5, xmm0);
    MOVAPS  (xmm6, xmm0);
    MOVAPS  (xmm7, xmm0);

    MOV     (rax, Imm64ff(0.0f, 0.0f));
    MOVQ    (xmm8,  rax);
    MOVLHPS (xmm8,  xmm0);
    MOVAPS  (xmm8,  xmm0);
    MOVAPS  (xmm10, xmm0);
    MOVAPS  (xmm11, xmm0);
    MOVAPS  (xmm12, xmm0);
    MOVAPS  (xmm13, xmm0);
    MOVAPS  (xmm14, xmm0);
    MOVAPS  (xmm15, xmm0);

    MOV     (rdi, ImmAddr(dataEnd() - memory_page_size()));
    MOV     (rsi, Imm32(0));
}

void runMethod(void (TestJitPerf::*method)(), const char* str)
{
    rewindCode();
    auto fun = (void(*)()) codeEnd();

    initRegs();

    MOV(rcx, Imm32(1000));

    getBeginTime();
    JumpLabel loop; mark(loop);
    (this->*method)();
    SUB(rcx, Imm32(1));
    JNZ(loop);
    getEndTime();

    RET();

    fun();
    cout << time() << str;
}

void empty()
{
}

void mul1()
{
    MULPS(xmm0, xmm1);
    MULPS(xmm0, xmm2);
    MULPS(xmm0, xmm3);
    MULPS(xmm0, xmm4);
    MULPS(xmm0, xmm5);
}

void mul2()
{
    MULPS(xmm0, xmm5);
    MULPS(xmm1, xmm5);
    MULPS(xmm2, xmm5);
    MULPS(xmm3, xmm5);
    MULPS(xmm4, xmm5);
}

void mul3()
{
    MULPS(xmm0, xmm1);
    XORPS(xmm6, xmm1);
    XORPS(xmm6, xmm2);
    XORPS(xmm6, xmm3);
    XORPS(xmm6, xmm4);
    XORPS(xmm6, xmm5);

    MULPS(xmm0, xmm2);
    XORPS(xmm6, xmm1);
    XORPS(xmm6, xmm2);
    XORPS(xmm6, xmm3);
    XORPS(xmm6, xmm4);
    XORPS(xmm6, xmm5);

    MULPS(xmm0, xmm3);
    XORPS(xmm6, xmm1);
    XORPS(xmm6, xmm2);
    XORPS(xmm6, xmm3);
    XORPS(xmm6, xmm4);
    XORPS(xmm6, xmm5);

    MULPS(xmm0, xmm4);
    XORPS(xmm6, xmm1);
    XORPS(xmm6, xmm2);
    XORPS(xmm6, xmm3);
    XORPS(xmm6, xmm4);
    XORPS(xmm6, xmm5);

    MULPS(xmm0, xmm5);
    XORPS(xmm6, xmm1);
    XORPS(xmm6, xmm2);
    XORPS(xmm6, xmm3);
    XORPS(xmm6, xmm4);
    XORPS(xmm6, xmm5);
}

void xor51()
{
    XORPS(xmm0, xmm1);
    XORPS(xmm0, xmm2);
    XORPS(xmm0, xmm3);
    XORPS(xmm0, xmm4);
    XORPS(xmm0, xmm5);
}

void xor52()
{
    XORPS(xmm0, xmm5);
    XORPS(xmm1, xmm5);
    XORPS(xmm2, xmm5);
    XORPS(xmm3, xmm5);
    XORPS(xmm4, xmm5);
}

void mulxor4()
{
    MULPS(xmm0, xmm1);
    XORPS(xmm2, xmm3);
    XORPS(xmm2, xmm4);
    XORPS(xmm2, xmm5);
    XORPS(xmm2, xmm6);
}

void mulxor9()
{
    MULPS(xmm0, xmm1);
    XORPS(xmm2, xmm3);
    XORPS(xmm2, xmm4);
    XORPS(xmm2, xmm5);
    XORPS(xmm2, xmm6);
    XORPS(xmm2, xmm7);
    XORPS(xmm2, xmm3);
    XORPS(xmm2, xmm4);
    XORPS(xmm2, xmm5);
    XORPS(xmm2, xmm6);
}

void loadxor()
{
    auto data = (float*)(dataEnd() - memory_page_size());
    for(int i=0; i<16; i++)
        data[i] = float(i);
    MOVAPS (xmm0, Mem128(data));
    MOVAPS (xmm1, Mem128(data + 4));
    MOVAPS (xmm2, Mem128(data + 8));
    XORPS  (xmm0, xmm1);
    XORPS  (xmm0, xmm2);
}

void loadmul1()
{
    auto data = (float*)(dataEnd() - memory_page_size());
    for(int i=0; i<32; i++)
        data[i] = float(i);
    MULPS (xmm0, Mem128(data));
    MULPS (xmm0, Mem128(data + 4));
    MULPS (xmm0, Mem128(data + 8));
    MULPS (xmm0, Mem128(data + 12));
    MULPS (xmm0, Mem128(data + 16));
}

void loadmul2()
{
    auto data = (float*)(dataEnd() - memory_page_size());
    for(int i=0; i<32; i++)
        data[i] = float(i);
    MULPS (xmm0, Mem128(data));
    MULPS (xmm1, Mem128(data + 4));
    MULPS (xmm2, Mem128(data + 8));
    MULPS (xmm3, Mem128(data + 12));
    MULPS (xmm4, Mem128(data + 16));
}

void loadmul3()
{
    auto data = (float*)(dataEnd() - memory_page_size());
    for(int i=0; i<32; i++)
        data[i] = float(i);
    MULPS (xmm0, Base(rdi) + Index(rsi));
    MULPS (xmm1, Base(rdi) + Index(rsi) + Disp(16));
    MULPS (xmm2, Base(rdi) + Index(rsi) + Disp(32));
    MULPS (xmm3, Base(rdi) + Index(rsi) + Disp(48));
    MULPS (xmm4, Base(rdi) + Index(rsi) + Disp(64));
}

void lmulw1()
{
    auto data = (float*)(dataEnd() - memory_page_size());
    for(int i=0; i<32; i++)
        data[i] = float(i);

    MULPS (xmm0, Mem128(data));
    MOVAPS(Mem128(data), xmm0);

    MULPS (xmm0, Mem128(data + 4));
    MOVAPS(Mem128(data + 4), xmm0);

    MULPS (xmm0, Mem128(data + 8));
    MOVAPS(Mem128(data + 8), xmm0);

    MULPS (xmm0, Mem128(data + 12));
    MOVAPS(Mem128(data + 12), xmm0);

    MULPS (xmm0, Mem128(data + 16));
    MOVAPS(Mem128(data + 16), xmm0);
}

void lmulw2()
{
    auto data = (float*)(dataEnd() - memory_page_size());
    for(int i=0; i<32; i++)
        data[i] = float(i);

    MOVAPS(xmm0, Mem128(data));
    MULPS (xmm0, xmm1);
    MOVAPS(Mem128(data + 20), xmm0);

    MOVAPS(xmm0, Mem128(data + 4));
    MULPS (xmm0, xmm1);
    MOVAPS(Mem128(data + 24), xmm0);

    MOVAPS(xmm0, Mem128(data + 8));
    MULPS (xmm0, xmm1);
    MOVAPS(Mem128(data + 28), xmm0);

    MOVAPS(xmm0, Mem128(data + 12));
    MULPS (xmm0, xmm1);
    MOVAPS(Mem128(data + 32), xmm0);

    MOVAPS(xmm0, Mem128(data + 16));
    MULPS (xmm0, xmm1);
    MOVAPS(Mem128(data + 36), xmm0);
}

void run()
{
    cout << "empty,mul1,mul2,mul3,xor51,xor52,mulxor4,mulxor9,loadxor,loadmul1,loadmul2,loadmul3,lmulw1,lmulw2\n";
    for(int i=0; i<16; i++)
    {
        runMethod(&TestJitPerf::empty,    ",");
        runMethod(&TestJitPerf::mul1,     ",");
        runMethod(&TestJitPerf::mul2,     ",");
        runMethod(&TestJitPerf::mul3,     ",");
        runMethod(&TestJitPerf::xor51,    ",");
        runMethod(&TestJitPerf::xor52,    ",");
        runMethod(&TestJitPerf::mulxor4,  ",");
        runMethod(&TestJitPerf::mulxor9,  ",");
        runMethod(&TestJitPerf::loadxor,  ",");
        runMethod(&TestJitPerf::loadmul1, ",");
        runMethod(&TestJitPerf::loadmul2, ",");
        runMethod(&TestJitPerf::loadmul3, ",");
        runMethod(&TestJitPerf::lmulw1,   ",");
        runMethod(&TestJitPerf::lmulw2,   "\n");
    }
}

};

int main()
{
    TestJitPerf().run();
    return 0;
}
