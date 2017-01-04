/* Test overall operation.
 *
 * Test REX prefix. Incorrect prefix order may cause cpu to ignore REX prefix altogether
 * thus encoding the wrong register.
 * Checking for rax being encoded instead r8 and rcx instead of r9.
 */

#include "jit.hpp"
#include <iostream>

using namespace std;
using namespace r64fx;

template<typename T1, typename T2> bool expect_eq(T1 expected, T2 got)
{
    if(expected != got)
    {
        std::cout << "Expected " << expected << ", Got " << got << "!\n";
        return false;
    }
    return true;
}
#define R64FX_EXPECT_EQ(expected, got) { auto evaled = (got); if(!expect_eq(expected, evaled)) return false; }


typedef long (*JitFun)();

void* g_data = nullptr;

bool test_mov(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    auto buff = (long int*) g_data;
    auto a = buff;
    auto b = buff + 1;

    srand(time(NULL));

    cout << "mov(GPR64, Imm32)\n";
    {
        int num = rand();
        as.rewindIp();
        as.mov(rax, Imm32S(num));
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(GPR64, Imm32) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.mov(r8,  Imm32S(num2));
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun())
    }

    cout << "mov(GPR64, Imm64)\n";
    {
        long int num = rand();
        as.rewindIp();
        as.mov(rax, Imm64S(num));
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(GPR64, Imm64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindIp();
        as.mov(rax, Imm64S(num1));
        as.mov(r8,  Imm64S(num2));
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "mov(GPR64, GPR64)\n";
    {
        int num = rand();
        as.rewindIp();
        as.mov(rcx, Imm32S(num));
        as.mov(rax, rcx);
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(GPR64, GPR64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        int num3 = rand();
        as.rewindIp();
        as.mov(r9,  Imm32S(num1));
        as.mov(rdx, Imm32S(num2));
        as.mov(rcx, Imm32S(num3));
        as.mov(rax, r9);
        as.mov(r8,  rdx);
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "mov(GPR64, Mem64)\n";
    {
        *a = rand();
        as.rewindIp();
        as.mov(rax, Mem64(a));
        as.ret();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "mov(GPR64, Mem64) rex\n";
    {
        *a = rand();
        *b = rand();
        as.rewindIp();
        as.mov(rax, Mem64(a));
        as.mov(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "mov(Mem64, GPR64)\n";
    {
        int num = rand();
        *a = rand();
        as.rewindIp();
        as.mov(rcx, Imm32S(num));
        as.mov(Mem64(a), rcx);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(num, *a);
    }

    cout << "mov(Mem64, GPR64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        *a = 0;
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.mov(r8,  Imm32S(num2));
        as.mov(Mem64(a), r8);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(num2, *a);
    }

    cout << "\n";
    return true;
}


bool test_add(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    auto buff = (long int*) g_data;
    auto a = buff;
    auto b = buff + 1;
    auto c = buff + 2;
    auto d = buff + 3;

    srand(time(NULL));

    cout << "add(GPR64, Imm32) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int sum = num1 + num2;
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.add(rax, Imm32S(num2));
        as.mov(r8,  Imm32S(0));
        as.add(r8,  Imm32S(num3)); //Corrupt dst. Rex encodes rax instead of r8.
        as.ret();
        R64FX_EXPECT_EQ(sum, jitfun());
    }

    cout << "add(GPR64, GPR64) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int sum = num1 + num2;
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.mov(r9,  Imm32S(num2));
        as.mov(rcx, Imm32S(num3));
        as.add(rcx, rdx);
        as.add(rax, r9);
        as.add(r8,  rdx);
        as.ret();
        R64FX_EXPECT_EQ(sum, jitfun());
    }

    cout << "add(GPR64, Mem64) + rex\n";
    {
        *a = rand();
        *b = rand();
        long sum = *a + *b;
        as.rewindIp();
        as.mov(rax, Imm64S(*b));
        as.add(rax, Mem64(a));
        as.add(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(sum, jitfun());
    }

    cout << "add(Mem64, GPR64) + rex\n";
    {
        *a = 0;
        long num1 = rand();
        as.rewindIp();
        as.mov(r8,  Imm32S(num1));
        as.mov(rax, Imm32S(rand()));
        as.add(Mem64(a), r8);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(num1, *a);
    }

    cout << "add(GPR64, Base, Disp8) + rex\n";
    {
        *a = rand();
        *b = rand();
        *c = rand();
        *d = rand();
        as.rewindIp();
        as.mov(rax, Imm32S(0));
        as.mov(r10, ImmAddr(a));
        as.mov(rdx, ImmAddr(c));
        as.add(rax, Base(r10), Disp8(8));
        as.add(r8,  Base(r10), Disp8(8));
        as.ret();
        R64FX_EXPECT_EQ(*b, jitfun());
    }

    cout << "add(Base, Disp8, GPR64) + rex\n";
    {
        *a = rand();
        *b = rand();
        *c = rand();
        *d = rand();
        long int num = rand();
        long int sum = num + *d;
        as.rewindIp();
        as.mov(rax, Imm64S(num + 1));
        as.mov(r8,  Imm64S(num));
        as.mov(rdx, ImmAddr(a));
        as.mov(r10, ImmAddr(c));
        as.add(Base(r10), Disp8(8), r8);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(sum, *d);
    }

    cout << "\n";
    return true;
}


bool test_sub(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    auto buff = (long int*) g_data;
    auto a = buff;
    auto b = buff + 1;
    auto c = buff + 2;
    auto d = buff + 3;

    srand(time(NULL));

    cout << "sub(GPR64, Imm32) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int dif = num1 - num2;
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.sub(rax, Imm32S(num2));
        as.mov(r8,  Imm32S(0));
        as.sub(r8,  Imm32S(num3)); //Corrupt dst. Rex encodes rax instead of r8.
        as.ret();
        R64FX_EXPECT_EQ(dif, jitfun());
    }

    cout << "sub(GPR64, GPR64) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int dif = num1 - num2;
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.mov(r9,  Imm32S(num2));
        as.mov(rcx, Imm32S(num3));
        as.sub(rcx, rdx);
        as.sub(rax, r9);
        as.sub(r8,  rdx);
        as.ret();
        R64FX_EXPECT_EQ(dif, jitfun());
    }

    cout << "sub(GPR64, Mem64) + rex\n";
    {
        *a = rand();
        *b = rand();
        long dif = *b - *a;
        as.rewindIp();
        as.mov(rax, Imm64S(*b));
        as.sub(rax, Mem64(a));
        as.sub(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(dif, jitfun());
    }

    cout << "sub(Mem64, GPR64) + rex\n";
    {
        *a = 0;
        long num1 = rand();
        as.rewindIp();
        as.mov(r8,  Imm32S(num1));
        as.mov(rax, Imm32S(rand()));
        as.sub(Mem64(a), r8);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(-num1, *a);
    }

    cout << "sub(GPR64, Base, Disp8) + rex\n";
    {
        *a = rand();
        *b = rand();
        *c = rand();
        *d = rand();
        as.rewindIp();
        as.mov(rax, Imm32S(0));
        as.mov(r10, ImmAddr(a));
        as.mov(rdx, ImmAddr(c));
        as.sub(rax, Base(r10), Disp8(8));
        as.sub(r8,  Base(r10), Disp8(8));
        as.ret();
        R64FX_EXPECT_EQ(-(*b), jitfun());
    }

    cout << "sub(Base, Disp8, GPR64) + rex\n";
    {
        *a = rand();
        *b = rand();
        *c = rand();
        *d = rand();
        long int num = rand();
        long int dif = *d - num;
        as.rewindIp();
        as.mov(rax, Imm64S(num + 1));
        as.mov(r8,  Imm64S(num));
        as.mov(rdx, ImmAddr(a));
        as.mov(r10, ImmAddr(c));
        as.sub(Base(r10), Disp8(8), r8);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(dif, *d);
    }

    cout << "\n";
    return true;
}


bool test_push_pop(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    cout << "push & pop\n";
    {
        long num = rand();
        as.rewindIp();
        as.mov(r9, Imm64S(num));
        as.push(r9);
        as.pop(rax);
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "\n";
    return true;
}


template<typename T> bool vec4_eq(T* a, T* b)
{
    return (a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]) && (a[3] == b[3]);
}


bool test_sse(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    cout << "movaps\n";
    {
        float* buff = (float*) g_data;
        auto a = buff;
        auto b = buff + 4;
        auto c = buff + 8;
        auto d = buff + 12;

        for(int i=0; i<4; i++)
        {
            a[i] = float(rand());
            b[i] = float(rand());
            c[i] = float(rand());
            d[i] = float(rand());
        }

        as.rewindIp();
        as.movaps(xmm0, Mem128(a));
        as.movaps(xmm8, Mem128(b));
        as.movaps(xmm1, xmm8);
        as.movaps(xmm9, xmm0);
        as.movaps(Mem128(c), xmm1);
        as.movaps(Mem128(d), xmm9);
        as.ret();

        jitfun();

        if(!vec4_eq(c, b) || !vec4_eq(a, d))
        {
            return false;
        }
    }

    cout << "movups + rex\n";
    {
        float* buff = (float*) g_data;
        auto a = buff + 1;
        auto b = buff + 5;
        auto c = buff + 9;
        auto d = buff + 13;

        for(int i=0; i<4; i++)
        {
            a[i] = float(rand());
            b[i] = float(rand());
            c[i] = float(rand());
            d[i] = float(rand());
        }

        as.rewindIp();
        as.movups(xmm0, Mem128(a));
        as.movups(xmm8, Mem128(b));
        as.movups(xmm1, xmm8);
        as.movups(xmm9, xmm0);
        as.movups(Mem128(c), xmm1);
        as.movups(Mem128(d), xmm9);
        as.ret();

        jitfun();

        if(!vec4_eq(c, b) || !vec4_eq(a, d))
        {
            return false;
        }
    }

    cout << "(add|sub|mul|div)ps + rex\n";
    {
        float* buff = (float*) g_data;
        auto a = buff + 4;
        auto b = buff + 8;
        auto c = buff + 12;
        auto d = buff + 16;
        auto f = buff + 20;
        auto r = buff + 24;
        auto n = buff + 28;

        for(int i=0; i<4; i++)
        {
            n[i] = 0.0f;
            a[i] = float(rand());
            b[i] = float(rand());
            c[i] = float(rand());
            do{ d[i] = float(rand()); } while(d[i] == 0.0f);
            r[i] = ((((a[i] + a[i] + a[i] - b[i] - b[i] - b[i]) * c[i] * c[i] * c[i]) / d[i]) / d[i]) / d[i];
            f[i] = float(rand());
        }

        as.rewindIp();

        as.movaps(xmm0,  Mem128(n));
        as.movaps(xmm1,  Mem128(f));
        as.movaps(xmm2,  Mem128(n));
        as.movaps(xmm3,  Mem128(n));

        as.movaps(xmm8,  Mem128(n));
        as.movaps(xmm9,  Mem128(n));
        as.movaps(xmm10, Mem128(n));
        as.movaps(xmm11, Mem128(n));

        as.mov(r9, ImmAddr(buff));

        as.addps(xmm8, Mem128(a));
        as.addps(xmm0, Mem128(f));
        as.addps(xmm8, Base(r9), Disp8(4 * 4));
        as.movaps(xmm9, Mem128(a));
        as.addps(xmm8, xmm9);
        as.addps(xmm0, xmm1);

        as.subps(xmm8, Mem128(b));
        as.subps(xmm0, Mem128(f));
        as.subps(xmm8, Base(r9), Disp8(4 * 8));
        as.movaps(xmm9, Mem128(b));
        as.subps(xmm8, xmm9);
        as.subps(xmm0, xmm1);

        as.mulps(xmm8, Mem128(c));
        as.mulps(xmm0, Mem128(f));
        as.mulps(xmm8, Base(r9), Disp8(4 * 12));
        as.movaps(xmm9, Mem128(c));
        as.mulps(xmm8, xmm9);
        as.mulps(xmm0, xmm1);

        as.divps(xmm8, Mem128(d));
        as.divps(xmm0, Mem128(f));
        as.divps(xmm8, Base(r9), Disp8(4 * 16));
        as.movaps(xmm9, Mem128(d));
        as.divps(xmm8, xmm9);
        as.divps(xmm0, xmm1);

        as.movaps(Mem128(n), xmm8);
        as.ret();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "\n";
    return true;
}


int main()
{
    g_data = alloc_aligned_memory(memory_page_size(), memory_page_size());
    for(int i=0; i<memory_page_size(); i++)
    {
        auto buff = (unsigned char*) g_data;
        buff[i] = 0;
    }

    CodeBuffer codebuff;
    Assembler as(&codebuff);

    bool ok =
        test_mov(as) &&
        test_add(as) &&
        test_sub(as) &&
        test_sse(as) &&
        test_push_pop(as)
    ;

    free(g_data);

    if(ok)
    {
        cout << "OK\n";
        return 0;
    }
    else
    {
        cout << "Fail!\n";
        return 1;
    }
}