/* Test overall operation.
 *
 * Test REX prefix. Incorrect prefix order may cause cpu to ignore REX prefix altogether
 * thus encoding a wrong register.
 * Checking for rax being encoded instead r8, rcx instead of r9 etc...
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

template<typename T> bool vec4_eq(T* a, T* b)
{
    return (a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]) && (a[3] == b[3]);
}

template<typename T> inline void dump(T* v)
{
    cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";
}


typedef long (*JitFun)();


bool test_buffers(Assembler &as)
{
    constexpr unsigned char* null_ptr = nullptr;

    cout << "buffers\n";
    as.resize(1, 1);
    as.resize(1, 0);
    as.resize(0, 1);
    as.resize(1, 1);
    R64FX_EXPECT_EQ(true, as.dataBegin() == as.codeBegin());
    as.resize(0, 0);
    R64FX_EXPECT_EQ(0UL, as.dataPageCount());
    R64FX_EXPECT_EQ(0UL, as.codePageCount());
    R64FX_EXPECT_EQ(null_ptr, as.dataBegin());
    R64FX_EXPECT_EQ(null_ptr, as.codeBegin());
    R64FX_EXPECT_EQ(null_ptr, as.codeEnd());
    as.nop(1);
    R64FX_EXPECT_EQ(0UL, as.dataPageCount());
    R64FX_EXPECT_EQ(1UL, as.codePageCount());
    R64FX_EXPECT_EQ(5678, as.growData(5678));
    R64FX_EXPECT_EQ((unsigned long)(5678 / memory_page_size() + 1), as.dataPageCount());

    /* Check code & data relocations! */
    as.rewindData();
    as.growData(16);
    R64FX_EXPECT_EQ(long(as.dataBegin()+16), long(as.codeBegin()));
    ((long*)(as.dataBegin()))[0] = 0x2AAAAAAAAAAAAAAAL;
    ((long*)(as.dataBegin()))[1] = 0x5555555555555555L;
    as.rewindCode();

    //Add 1 to 0x2AAAAAAAAAAAAAAAL
    as.mov(rax, Imm32(1));
    as.add(Mem64(as.dataBegin()), rax);
    as.nop(memory_page_size() + (rand() & 0xF));

    as.mov(rdx, Imm32(100));
    as.nop(memory_page_size() + (rand() & 0xF));

    as.mov(rcx, Imm32(10));
    as.nop(memory_page_size() + (rand() & 0xF));

    //Sub 16 from 0x5555555555555555L
    as.mov(rax, Imm32(16));
    as.sub(Mem64(as.dataBegin() + 8), rax);
    as.nop(memory_page_size() + (rand() & 0xF));

    as.mov(rax, Imm32(1));
    as.nop(memory_page_size() + (rand() & 0xF));

    as.add(rax, rcx);
    as.nop(memory_page_size() + (rand() & 0xF));
    as.add(rax, rdx);
    as.ret();

    R64FX_EXPECT_EQ(111, ((JitFun)as.codeBegin())());
    R64FX_EXPECT_EQ(0x2AAAAAAAAAAAAAABL, ((long*)(as.dataBegin()))[0]); // +1
    R64FX_EXPECT_EQ(0x5555555555555545L, ((long*)(as.dataBegin()))[1]); // -16

    as.resize(1, 1);
    cout << "\n";
    return true;
}


bool test_mov(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    as.rewindData();
    as.growData(2 * sizeof(long));
    auto buff = (long*) as.dataBegin();
    auto a = buff;
    auto b = buff + 1;

    cout << "mov(GPR64, Imm32)\n";
    {
        int num = rand();
        as.rewindCode();
        as.mov(rax, Imm32(num));
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(GPR64, Imm32) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindCode();
        as.mov(rax, Imm32(num1));
        as.mov(r8,  Imm32(num2));
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun())
    }

    cout << "mov(GPR64, Imm64)\n";
    {
        long int num = rand();
        as.rewindCode();
        as.mov(rax, Imm64(num));
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(GPR64, Imm64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindCode();
        as.mov(rax, Imm64(num1));
        as.mov(r8,  Imm64(num2));
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "mov(GPR64, GPR64)\n";
    {
        int num = rand();
        as.rewindCode();
        as.mov(rcx, Imm32(num));
        as.mov(rax, rcx);
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(GPR64, GPR64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        int num3 = rand();
        as.rewindCode();
        as.mov(r9,  Imm32(num1));
        as.mov(rdx, Imm32(num2));
        as.mov(rcx, Imm32(num3));
        as.mov(rax, r9);
        as.mov(r8,  rdx);
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "mov(GPR64, Mem64)\n";
    {
        *a = rand();
        as.rewindCode();
        as.mov(rax, Mem64(a));
        as.ret();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "mov(GPR64, Mem64) rex\n";
    {
        *a = rand();
        *b = rand();
        as.rewindCode();
        as.mov(rax, Mem64(a));
        as.mov(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "mov(Mem64, GPR64)\n";
    {
        int num = rand();
        *a = rand();
        as.rewindCode();
        as.mov(rcx, Imm32(num));
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
        as.rewindCode();
        as.mov(rax, Imm32(num1));
        as.mov(r8,  Imm32(num2));
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

    as.rewindData();
    as.growData(4 * sizeof(long));
    auto buff = (long*) as.dataBegin();
    auto a = buff;
    auto b = buff + 1;
    auto c = buff + 2;
    auto d = buff + 3;

    cout << "add(GPR64, Imm32) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int sum = num1 + num2;
        as.rewindCode();
        as.mov(rax, Imm32(num1));
        as.add(rax, Imm32(num2));
        as.mov(r8,  Imm32(0));
        as.add(r8,  Imm32(num3)); //Corrupt dst. Rex encodes rax instead of r8.
        as.ret();
        R64FX_EXPECT_EQ(sum, jitfun());
    }

    cout << "add(GPR64, GPR64) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int sum = num1 + num2;
        as.rewindCode();
        as.mov(rax, Imm32(num1));
        as.mov(r9,  Imm32(num2));
        as.mov(rcx, Imm32(num3));
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
        as.rewindCode();
        as.mov(rax, Imm64(*b));
        as.add(rax, Mem64(a));
        as.add(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(sum, jitfun());
    }

    cout << "add(Mem64, GPR64) + rex\n";
    {
        *a = 0;
        long num1 = rand();
        as.rewindCode();
        as.mov(r8,  Imm32(num1));
        as.mov(rax, Imm32(rand()));
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
        as.rewindCode();
        as.mov(rax, Imm32(0));
        as.mov(r10, ImmAddr(a));
        as.mov(rdx, ImmAddr(c));
        as.add(rax, Base(r10) + Disp(8));
        as.add(r8,  Base(r10) + Disp(8));
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
        as.rewindCode();
        as.mov(rax, Imm64(num + 1));
        as.mov(r8,  Imm64(num));
        as.mov(rdx, ImmAddr(a));
        as.mov(r10, ImmAddr(c));
        as.add(Base(r10) + Disp(8), r8);
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

    as.rewindData();
    as.growData(4 * sizeof(long));
    auto buff = (long*) as.dataBegin();
    auto a = buff;
    auto b = buff + 1;
    auto c = buff + 2;
    auto d = buff + 3;

    cout << "sub(GPR64, Imm32) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int dif = num1 - num2;
        as.rewindCode();
        as.mov(rax, Imm32(num1));
        as.sub(rax, Imm32(num2));
        as.mov(r8,  Imm32(0));
        as.sub(r8,  Imm32(num3)); //Corrupt dst. Rex encodes rax instead of r8.
        as.ret();
        R64FX_EXPECT_EQ(dif, jitfun());
    }

    cout << "sub(GPR64, GPR64) + rex\n";
    {
        int num1 = rand() & 0xFFFF;
        int num2 = rand() & 0xFFFF;
        int num3 = rand() & 0xFFFF;
        int dif = num1 - num2;
        as.rewindCode();
        as.mov(rax, Imm32(num1));
        as.mov(r9,  Imm32(num2));
        as.mov(rcx, Imm32(num3));
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
        as.rewindCode();
        as.mov(rax, Imm64(*b));
        as.sub(rax, Mem64(a));
        as.sub(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(dif, jitfun());
    }

    cout << "sub(Mem64, GPR64) + rex\n";
    {
        *a = 0;
        long num1 = rand();
        as.rewindCode();
        as.mov(r8,  Imm32(num1));
        as.mov(rax, Imm32(rand()));
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
        as.rewindCode();
        as.mov(rax, Imm32(0));
        as.mov(r10, ImmAddr(a));
        as.mov(rdx, ImmAddr(c));
        as.sub(rax, Base(r10) + Disp(8));
        as.sub(r8,  Base(r10) + Disp(8));
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
        as.rewindCode();
        as.mov(rax, Imm64(num + 1));
        as.mov(r8,  Imm64(num));
        as.mov(rdx, ImmAddr(a));
        as.mov(r10, ImmAddr(c));
        as.sub(Base(r10) + Disp(8), r8);
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
        as.rewindCode();
        as.mov(r9, Imm64(num));
        as.push(r9);
        as.pop(rax);
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "\n";
    return true;
}


bool test_sse(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();
    as.rewindData();
    as.growData(64 * sizeof(float));

    cout << "movaps + rex\n";
    {
        float* buff = (float*) as.dataBegin();
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

        as.rewindCode();
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
        float* buff = (float*) as.dataBegin();
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

        as.rewindCode();
        as.movups(xmm0, Mem32(a));
        as.movups(xmm8, Mem32(b));
        as.movups(xmm1, xmm8);
        as.movups(xmm9, xmm0);
        as.movups(Mem32(c), xmm1);
        as.movups(Mem32(d), xmm9);
        as.ret();

        jitfun();

        if(!vec4_eq(c, b) || !vec4_eq(a, d))
        {
            return false;
        }
    }

    cout << "(add|sub|mul|div)ps + rex\n";
    {
        float* buff = (float*) as.dataBegin();
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

        as.rewindCode();

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
        as.addps(xmm8, Base(r9) + Disp(4 * 4));
        as.movaps(xmm9, Mem128(a));
        as.addps(xmm8, xmm9);
        as.addps(xmm0, xmm1);

        as.subps(xmm8, Mem128(b));
        as.subps(xmm0, Mem128(f));
        as.subps(xmm8, Base(r9) + Disp(4 * 8));
        as.movaps(xmm9, Mem128(b));
        as.subps(xmm8, xmm9);
        as.subps(xmm0, xmm1);

        as.mulps(xmm8, Mem128(c));
        as.mulps(xmm0, Mem128(f));
        as.mulps(xmm8, Base(r9) + Disp(4 * 12));
        as.movaps(xmm9, Mem128(c));
        as.mulps(xmm8, xmm9);
        as.mulps(xmm0, xmm1);

        as.divps(xmm8, Mem128(d));
        as.divps(xmm0, Mem128(f));
        as.divps(xmm8, Base(r9) + Disp(4 * 16));
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

    cout << "p(add|sub)d + rex\n";
    {
        int* buff = (int*) as.dataBegin();
        auto a = buff + 4;
        auto b = buff + 8;
        auto f = buff + 20;
        auto r = buff + 24;
        auto n = buff + 28;

        for(int i=0; i<4; i++)
        {
            n[i] = 0.0f;
            a[i] = rand();
            b[i] = rand();
            r[i] = a[i] + a[i] + a[i] - b[i] - b[i] - b[i];
            f[i] = rand();
        }

        as.rewindCode();

        as.movaps(xmm0,  Mem128(n));
        as.movaps(xmm1,  Mem128(f));
        as.movaps(xmm2,  Mem128(n));
        as.movaps(xmm3,  Mem128(n));

        as.movaps(xmm8,  Mem128(n));
        as.movaps(xmm9,  Mem128(n));
        as.movaps(xmm10, Mem128(n));
        as.movaps(xmm11, Mem128(n));

        as.mov(r9, ImmAddr(buff));

        as.paddd(xmm8, Mem128(a));
        as.paddd(xmm0, Mem128(f));
        as.paddd(xmm8, Base(r9) + Disp(4 * 4));
        as.movaps(xmm9, Mem128(a));
        as.paddd(xmm8, xmm9);
        as.paddd(xmm0, xmm1);

        as.psubd(xmm8, Mem128(b));
        as.psubd(xmm0, Mem128(f));
        as.psubd(xmm8, Base(r9) + Disp(4 * 8));
        as.movaps(xmm9, Mem128(b));
        as.psubd(xmm8, xmm9);
        as.psubd(xmm0, xmm1);

        as.movaps(Mem128(n), xmm8);
        as.ret();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "(and|andn|or|xor)ps + rex\n";
    {
        int* buff = (int*) as.dataBegin();
        auto a = buff + 4;
        auto b = buff + 8;
        auto c = buff + 12;
        auto d = buff + 16;
        auto f = buff + 20;
        auto r = buff + 24;
        auto n = buff + 28;
        auto u = buff + 32;

        int num = 1 + 4 + 16 + 32 + 128 + 1024 + 2048 + 8196;
        for(int i=0; i<4; i++)
        {
            n[i] = 0;
            u[i] = num;
            a[i] = rand();
            b[i] = rand();
            c[i] = rand();
            d[i] = rand();

            r[i] = u[i];
            r[i] = r[i] & a[i];
            r[i] += num;
            r[i] = r[i] & a[i];
            r[i] += num;
            r[i] = r[i] & a[i];
            r[i] += num;
            r[i] = (~r[i]) & b[i];
            r[i] += num;
            r[i] = (~r[i]) & b[i];
            r[i] += num;
            r[i] = (~r[i]) & b[i];
            r[i] += num;
            r[i] = r[i] | c[i];
            r[i] += num;
            r[i] = r[i] | c[i];
            r[i] += num;
            r[i] = r[i] | c[i];
            r[i] += num;
            r[i] = r[i] ^ d[i];
            r[i] += num;
            r[i] = r[i] ^ d[i];
            r[i] += num;
            r[i] = r[i] ^ d[i];
            r[i] += num;

            f[i] = rand();
        }

        as.rewindCode();

        as.movaps(xmm0,  Mem128(n));
        as.movaps(xmm1,  Mem128(f));
        as.movaps(xmm2,  Mem128(n));
        as.movaps(xmm3,  Mem128(n));

        as.movaps(xmm8,  Mem128(u));
        as.movaps(xmm9,  Mem128(u));
        as.movaps(xmm10, Mem128(u));
        as.movaps(xmm11, Mem128(u));

        as.mov(r9, ImmAddr(buff));

        as.andps(xmm8, Mem128(a));
        as.paddd(xmm8, Mem128(u));
        as.andps(xmm0, Mem128(f));
        as.andps(xmm8, Base(r9) + Disp(4 * 4));
        as.paddd(xmm8, Mem128(u));
        as.movaps(xmm9, Mem128(a));
        as.andps(xmm8, xmm9);
        as.paddd(xmm8, Mem128(u));
        as.andps(xmm0, xmm1);

        as.andnps(xmm8, Mem128(b));
        as.paddd(xmm8, Mem128(u));
        as.andnps(xmm0, Mem128(f));
        as.andnps(xmm8, Base(r9) + Disp(4 * 8));
        as.paddd(xmm8, Mem128(u));
        as.movaps(xmm9, Mem128(b));
        as.andnps(xmm8, xmm9);
        as.paddd(xmm8, Mem128(u));
        as.andnps(xmm0, xmm1);

        as.orps(xmm8, Mem128(c));
        as.paddd(xmm8, Mem128(u));
        as.orps(xmm0, Mem128(f));
        as.orps(xmm8, Base(r9) + Disp(4 * 12));
        as.paddd(xmm8, Mem128(u));
        as.movaps(xmm9, Mem128(c));
        as.orps(xmm8, xmm9);
        as.paddd(xmm8, Mem128(u));
        as.orps(xmm0, xmm1);

        as.xorps(xmm8, Mem128(d));
        as.paddd(xmm8, Mem128(u));
        as.xorps(xmm0, Mem128(f));
        as.xorps(xmm8, Base(r9) + Disp(4 * 16));
        as.paddd(xmm8, Mem128(u));
        as.movaps(xmm9, Mem128(d));
        as.xorps(xmm8, xmm9);
        as.paddd(xmm8, Mem128(u));
        as.xorps(xmm0, xmm1);

        as.movaps(Mem128(n), xmm8);
        as.ret();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "minps\n";
    {
        float* buff = (float*) as.dataBegin();
        auto a = buff;
        auto b = buff + 4;
        auto c = buff + 8;
        auto d = buff + 12;
        auto n = buff + 16;
        auto r = buff + 20;
        auto m = buff + 24;

        for(int i=0; i<4; i++)
        {
            m[i] = float(rand());
            a[i] = float(rand());
            b[i] = float(rand());
            c[i] = float(rand());
            d[i] = float(rand());
            r[i] = d[i];
            r[i] = (r[i] < a[i] ? r[i] : a[i]);
            r[i] += m[i];
            r[i] = (r[i] < b[i] ? r[i] : b[i]);
            r[i] *= m[i];
            r[i] = (r[i] < c[i] ? r[i] : c[i]);
            n[i] = 0;
        }

        as.rewindCode();
        as.movaps(xmm8, Mem128(d));
        as.minps(xmm8, Mem128(a));
        as.addps(xmm8, Mem128(m));
        as.movaps(xmm9, Mem128(b));
        as.minps(xmm8, xmm9);
        as.mulps(xmm8, Mem128(m));
        as.mov(rcx, ImmAddr(buff));
        as.minps(xmm8, Base(rcx) + Disp(4 * 8));
        as.movaps(Mem128(n), xmm8);
        as.ret();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "maxps\n";
    {
        float* buff = (float*) as.dataBegin();
        auto a = buff;
        auto b = buff + 4;
        auto c = buff + 8;
        auto d = buff + 12;
        auto n = buff + 16;
        auto r = buff + 20;
        auto m = buff + 24;

        for(int i=0; i<4; i++)
        {
            m[i] = float(rand());
            a[i] = float(rand());
            b[i] = float(rand());
            c[i] = float(rand());
            d[i] = float(rand());
            r[i] = d[i];
            r[i] = (r[i] > a[i] ? r[i] : a[i]);
            r[i] += m[i];
            r[i] = (r[i] > b[i] ? r[i] : b[i]);
            r[i] *= m[i];
            r[i] = (r[i] > c[i] ? r[i] : c[i]);
            n[i] = 0;
        }

        as.rewindCode();
        as.movaps(xmm8, Mem128(d));
        as.maxps(xmm8, Mem128(a));
        as.addps(xmm8, Mem128(m));
        as.movaps(xmm9, Mem128(b));
        as.maxps(xmm8, xmm9);
        as.mulps(xmm8, Mem128(m));
        as.mov(rcx, ImmAddr(buff));
        as.maxps(xmm8, Base(rcx) + Disp(4 * 8));
        as.movaps(Mem128(n), xmm8);
        as.ret();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "pshufd\n";
    {
        int* buff = (int*) as.dataBegin();
        auto a1 = buff;
        auto b1 = buff + 4;
        auto c1 = buff + 8;
        auto a2 = buff + 12;
        auto b2 = buff + 16;
        auto c2 = buff + 20;
        auto a3 = buff + 24;
        auto b3 = buff + 28;
        auto c3 = buff + 32;

        for(int i=0; i<4; i++)
        {
            a1[i] = rand();
            b1[i] = 0;

            a2[i] = rand();
            b2[i] = 0;

            a3[i] = rand();
            b3[i] = 0;
        }

        c1[3] = a1[0];
        c1[2] = a1[1];
        c1[1] = a1[2];
        c1[0] = a1[3];

        c2[3] = a2[1];
        c2[2] = a2[1];
        c2[1] = a2[1];
        c2[0] = a2[1];

        c3[3] = a3[0];
        c3[2] = a3[2];
        c3[1] = a3[0];
        c3[0] = a3[2];

        as.rewindCode();
        as.pshufd(xmm8, Mem128(a1), Shuf(3, 2, 1, 0));
        as.movaps(Mem128(b1), xmm8);

        as.movaps(xmm0, Mem128(a2));
        as.pshufd(xmm0, xmm0, Shuf(1, 1, 1, 1));
        as.movaps(Mem128(b2), xmm0);

        as.mov(rcx, ImmAddr(buff));
        as.pshufd(xmm5, Base(rcx) + Disp(4 * 24), Shuf(2, 0, 2, 0));
        as.movaps(Mem128(b3), xmm5);

        as.ret();
        jitfun();

        if(!vec4_eq(b1, c1) || !vec4_eq(b2, c2) || !vec4_eq(b3, c3))
        {
            return false;
        }
    }

    cout << "cmpltps\n";
    {
        float* buff = (float*) as.dataBegin();
        auto a = buff;
        auto b = buff + 4;
        auto c = (int*)(buff + 8);
        auto d = (int*)(buff + 12);
        for(int i=0; i<4; i++)
        {
            a[i] = float(rand() & 0xFF) * 0.5f;
            b[i] = float(rand() & 0xFF) * 0.5f;
            d[i] = (a[i] < b[i] ? -1 : 0);
        }

        as.rewindCode();

        as.movaps(xmm0, Mem128(a)); 
        as.movaps(xmm1, Mem128(b));
        as.cmpltps(xmm0, xmm1);
        as.movaps(Mem128(c), xmm0);

        as.ret();
        jitfun();

        if(!vec4_eq(c, d))
        {
            return false;
        }
    }

    cout << "\n";
    return true;
}


bool test_jumps(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    cout << "jnz\n";
    {
        JumpLabel loop, skip;

        as.rewindCode();
        as.mov(rax, Imm32(0));
        as.mov(rcx, Imm32(1234));
        as.mark(loop);
        as.add(rax, Imm32(2));

        as.jmp(skip);
        as.add(rax, Imm32(1));
        as.mark(skip);

        as.sub(rcx, Imm32(1));
        as.jnz(loop);
        as.ret();
        R64FX_EXPECT_EQ(2468, jitfun());
    }

    cout << "\n";
    return true;
}


bool test_sibd(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    cout << "[base + index * 8]\n";
    {
        as.rewindData();
        as.growData(16 * sizeof(long));
        auto buff = (long*) as.dataBegin();
        buff[10] = rand() & 0xFFFF;

        as.rewindCode();

        as.mov(rax, Imm32(0));
        as.mov(rcx, ImmAddr(buff));
        as.mov(rdx, Imm32(10));
        as.mov(rax, Base(rcx) + Index(rdx, 8));

        as.ret();
        R64FX_EXPECT_EQ(buff[10], jitfun());
    }

    cout << "[base + index * 8 + Disp8]\n";
    {
        as.rewindData();
        as.growData(16 * sizeof(long));
        auto buff = (long*) as.dataBegin();
        buff[11] = rand() & 0xFFFF;

        as.rewindCode();

        as.mov(rax, Imm32(0));
        as.mov(rcx, ImmAddr(buff));
        as.mov(rdx, Imm32(10));
        as.mov(rax, Base(rcx) + Index(rdx, 8) + Disp(8));

        as.ret();
        R64FX_EXPECT_EQ(buff[11], jitfun());
    }

    cout << "[base + index * 8 + Disp32]\n";
    {
        as.rewindData();
        as.growData((128) * sizeof(long));
        auto buff = (long*) as.dataBegin();
        buff[10 + 64] = rand() & 0xFFFF;

        as.rewindCode();

        as.mov(rax, Imm32(0));
        as.mov(rcx, ImmAddr(buff));
        as.mov(rdx, Imm32(10));
        as.mov(rax, Base(rcx) + Index(rdx, 8) + Disp(64 * 8));

        as.ret();
        R64FX_EXPECT_EQ(buff[10 + 64], jitfun());
    }

    cout << "[base + index * 4]\n";
    {
        as.rewindData();
        as.growData(32 * sizeof(float));
        auto buff = (float*)as.dataBegin();
        for(int i=0; i<32; i++)
            buff[i] = float(i);

        as.rewindCode();

        as.mov(rcx, ImmAddr(buff));
        as.mov(rdx, Imm32(4));
        as.movaps(xmm0, Base(rcx) + Index(rdx, 4));
        as.movaps(xmm8, Base(rcx) + Index(rdx, 8));
        as.movaps(Base(rcx), xmm0);

        as.ret();
        jitfun();

        if(!vec4_eq(buff, buff + 4))
        {
            return false;
        }
    }

    cout << "\n";
    return true;
}


int main()
{
    srand(time(NULL));

    Assembler as;
    as.resize(1, 1);

    bool ok =
        test_buffers(as) &&
        test_mov(as) &&
        test_push_pop(as) &&
        test_add(as) &&
        test_sub(as) &&
        test_sse(as) &&
        test_jumps(as) &&
        test_sibd(as)
    ;

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
