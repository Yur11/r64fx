/* Test overall operation.
 *
 * Test REX prefix. Incorrect prefix order may cause cpu to ignore REX prefix altogether
 * thus encoding a wrong register.
 * Checking for rax being encoded instead r8, rcx instead of r9 etc...
 */

#include "test.hpp"
#include "jit.hpp"
#include <iostream>

using namespace std;
using namespace r64fx;


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
    as.NOP(1);
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
    as.MOV(rax, Imm32(1));
    as.ADD(Mem64(as.dataBegin()), rax);
    as.NOP(memory_page_size() + (rand() & 0xF));

    as.MOV(rdx, Imm32(100));
    as.NOP(memory_page_size() + (rand() & 0xF));

    as.MOV(rcx, Imm32(10));
    as.NOP(memory_page_size() + (rand() & 0xF));

    //Sub 16 from 0x5555555555555555L
    as.MOV(rax, Imm32(16));
    as.SUB(Mem64(as.dataBegin() + 8), rax);
    as.NOP(memory_page_size() + (rand() & 0xF));

    as.MOV(rax, Imm32(1));
    as.NOP(memory_page_size() + (rand() & 0xF));

    as.ADD(rax, rcx);
    as.NOP(memory_page_size() + (rand() & 0xF));
    as.ADD(rax, rdx);
    as.RET();

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

    cout << "MOV(GPR32, Imm32)\n";
    {
        int num = rand();
        as.rewindCode();
        as.MOV(eax, Imm32(num));
        as.RET();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "MOV(GPR64, Imm32)\n";
    {
        int num = rand();
        as.rewindCode();
        as.MOV(rax, Imm32(num));
        as.RET();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "MOV(GPR64, Imm32) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindCode();
        as.MOV(rax, Imm32(num1));
        as.MOV(r8,  Imm32(num2));
        as.RET();
        R64FX_EXPECT_EQ(num1, jitfun())
    }

    cout << "MOV(GPR64, Imm64)\n";
    {
        long int num = rand();
        as.rewindCode();
        as.MOV(rax, Imm64(num));
        as.RET();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "MOV(GPR64, Imm64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindCode();
        as.MOV(rax, Imm64(num1));
        as.MOV(r8,  Imm64(num2));
        as.RET();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "MOV(GPR64, GPR64)\n";
    {
        int num = rand();
        as.rewindCode();
        as.MOV(rcx, Imm32(num));
        as.MOV(rax, rcx);
        as.RET();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "MOV(GPR64, GPR64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        int num3 = rand();
        as.rewindCode();
        as.MOV(r9,  Imm32(num1));
        as.MOV(rdx, Imm32(num2));
        as.MOV(rcx, Imm32(num3));
        as.MOV(rax, r9);
        as.MOV(r8,  rdx);
        as.RET();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "MOV(GPR64, Mem64)\n";
    {
        *a = rand();
        as.rewindCode();
        as.MOV(rax, Mem64(a));
        as.RET();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "MOV(GPR32, Mem32)\n";
    {
        auto aa = (int*)a;
        auto a1 = aa;
        auto a2 = aa + 1;
        *a1 = rand() & 0xFFFFFF;
        *a2 = rand() & 0xFFFFFF;
        as.rewindCode();
        as.MOV(eax, Mem32(a2));
        as.MOV(r8d, Mem32(a1));
        as.RET();
        R64FX_EXPECT_EQ(*a2, jitfun());
    }

    cout << "MOV(GPR64, Mem64) rex\n";
    {
        *a = rand();
        *b = rand();
        as.rewindCode();
        as.MOV(rax, Mem64(a));
        as.MOV(r8,  Mem64(b));
        as.RET();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "MOV(Mem64, GPR64)\n";
    {
        int num = rand();
        *a = rand();
        as.rewindCode();
        as.MOV(rcx, Imm32(num));
        as.MOV(Mem64(a), rcx);
        as.RET();
        jitfun();
        R64FX_EXPECT_EQ(num, *a);
    }

    cout << "MOV(Mem64, GPR64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        *a = 0;
        as.rewindCode();
        as.MOV(rax, Imm32(num1));
        as.MOV(r8,  Imm32(num2));
        as.MOV(Mem64(a), r8);
        as.RET();
        jitfun();
        R64FX_EXPECT_EQ(num2, *a);
    }

    {
        as.rewindData();
        as.growData(sizeof(int) * 4);
        auto ptr = (int*) as.dataBegin();

        cout << "MOV(GPR32, Mem32)\n";
        for(int i=0; i<4; i++)
            ptr[i] = rand();
        as.rewindCode();
        as.MOV(eax, Mem32(ptr + 1));
        as.RET();
        R64FX_EXPECT_EQ(ptr[1], jitfun());

        cout << "MOV(GPR32, Mem32) rex\n";
        for(int i=0; i<4; i++)
            ptr[i] = rand();
        as.rewindCode();
        as.MOV(eax, Mem32(ptr + 1));
        as.MOV(r8d, Mem32(ptr + 2));
        as.RET();
        R64FX_EXPECT_EQ(ptr[1], jitfun());

        cout << "MOV(Mem32, GPR)\n";
        for(int i=0; i<4; i++)
            ptr[i] = rand();
        as.rewindCode();
        as.MOV(eax, Imm32(ptr[0]));
        as.MOV(Mem32(ptr + 1), eax);
        as.RET();
        jitfun();
        R64FX_EXPECT_EQ(ptr[0], ptr[1]);
    }

    cout << "\n";
    return true;
}


template<
    void (Assembler::*gpr64_imm32) (GPR64, Imm32),
    void (Assembler::*gpr64_imm8 ) (GPR64, Imm8 ),
    void (Assembler::*gpr64_gpr64) (GPR64, GPR64),
    void (Assembler::*gpr64_mem64) (GPR64, Mem64),
    void (Assembler::*gpr64_sibd ) (GPR64, SIBD ),
    void (Assembler::*mem64_gpr64) (Mem64, GPR64),
    void (Assembler::* sibd_gpr64) (SIBD,  GPR64)
> bool test_gpr_inst(const char* name, Assembler &as, int (*expected)(int a, int b))
{
    auto jitfun = (JitFun) as.codeBegin();

    as.rewindData();
    as.growData(4 * sizeof(long));

    cout << name << "(GPR64, Imm32)\n";
    {
        int a = rand() & 0xFFFFFF;
        int b = rand() & 0xFFFFFF;
        int c = rand() & 0xFFFFFF;

        as.rewindCode();
        as.MOV(rax, Imm32(a));
        (as.*gpr64_imm32)(rax, Imm32(b));
        (as.*gpr64_imm32)(r8,  Imm32(c));
        as.RET();
        R64FX_EXPECT_EQ(expected(a, b), jitfun());
    }

    cout << name << "(GPR64, Imm8 )\n";
    {
        char a = rand() & 0xFF;
        char b = rand() & 0xFF;
        char c = rand() & 0xFF;

        as.rewindCode();
        as.MOV(rax, Imm32(a));
        (as.*gpr64_imm8)(rax, Imm8(b));
        (as.*gpr64_imm8)(r8,  Imm8(c));
        as.RET();
        R64FX_EXPECT_EQ(expected(a, b), jitfun());
    }

    cout << name << "(GPR64, GPR64)\n";
    {
        int a = rand() & 0xFFFFFF;
        int b = rand() & 0xFFFFFF;
        int c = rand() & 0xFFFFFF;

        as.rewindCode();
        as.MOV(rax, Imm32(a));
        as.MOV(rcx, Imm32(b));
        as.MOV(rdx, Imm32(c));
        (as.*gpr64_gpr64)(rax, rcx);
        (as.*gpr64_gpr64)(r8,  rdx);
        as.RET();
        R64FX_EXPECT_EQ(expected(a, b), jitfun());
    }

    cout << name << "(GPR64, Mem64)\n";
    {
        auto buff = (long*) as.dataBegin();
        for(int i=0; i<4; i++)
        {
            buff[i] = rand() & 0xFFFFFF;
        }

        as.rewindCode();
        as.MOV(rax, Imm32(0));
        as.MOV(rax, Mem64(buff + 0));
        (as.*gpr64_mem64)(rax, Mem64(buff + 1));
        (as.*gpr64_mem64)(r8,  Mem64(buff + 2));
        as.RET();
        R64FX_EXPECT_EQ(expected(buff[0], buff[1]), jitfun());
    }

    cout << name << "(GPR64, SIBD )\n";
    {
        auto buff = (long*) as.dataBegin();
        for(int i=0; i<4; i++)
        {
            buff[i] = rand() & 0xFFFFFF;
        }

        as.rewindCode();
        as.MOV(rax, Imm32(0));
        as.MOV(r8,  Imm32(1));
        as.MOV(rax, Mem64(buff + 0));
        as.MOV(rdx, ImmAddr(buff + 1));
        as.MOV(rcx, Imm32(1));
        (as.*gpr64_sibd)(rax, Base(rdx) + Index(rcx) * 8 + Disp(8));
        (as.*gpr64_sibd)(r8, Base(rdx) + Index(rcx) * 8 + Disp(8));
        as.RET();
        R64FX_EXPECT_EQ(expected(buff[0], buff[3]), jitfun());
    }

    cout << "\n";
    return true;
}


bool test_gpr_instrs(Assembler &as)
{
#define R64FX_LIST7(a) a,a,a,a,a,a,a
#define R64FX_TEST_GPR_INSTR(name, op) test_gpr_inst<R64FX_LIST7(&Assembler::name)>(#name, as, [](int a, int b){ return a op b; })
    return
        R64FX_TEST_GPR_INSTR(ADD, +) &&
        R64FX_TEST_GPR_INSTR(SUB, -) &&
        R64FX_TEST_GPR_INSTR(XOR, ^) &&
        R64FX_TEST_GPR_INSTR(AND, &) &&
        R64FX_TEST_GPR_INSTR(OR,  |);
#undef R64FX_LIST7
#undef R64FX_TEST_GPR_INSTR
}


template<
    typename IntT,
    void (Assembler::*shift1)     (GPR64 gpr),
    void (Assembler::*shift_imm8) (GPR64 gpr, Imm8)
>
bool test_shift_instr(const char* name, Assembler &as, IntT (*expected)(IntT num, IntT shift))
{
    auto jitfun = (JitFun) as.codeBegin();

    cout << name << "(GPR, 1)\n";
    {
        IntT num = rand() & 0xFFFF;
        as.rewindCode();
        as.MOV(rax, Imm32(num));
        (as.*shift1)(rax);
        (as.*shift1)(r8);
        as.RET();
        R64FX_EXPECT_EQ(expected(num, 1), jitfun());
    }

    cout << name << "(GPR, Imm8)\n";
    {
        IntT num = rand() & 0xFF;
        IntT shift = ((IntT)(rand() & 0x3)) + 1;
        as.rewindCode();
        as.MOV(rax, Imm32(num));
        (as.*shift_imm8)(rax, Imm8(shift));
        (as.*shift_imm8)(r8,  Imm8(shift));
        as.RET();
        R64FX_EXPECT_EQ(expected(num, shift), jitfun());
    }

    return true;
}


bool test_shift_instrs(Assembler &as)
{
#define R64FX_LIST2(a) a,a
#define R64FX_TEST_SHIFT_INSTR(type, name, op) test_shift_instr<type, R64FX_LIST2(&Assembler::name)>(#name, as, [](type num, type shift){ return num op shift; })
    return
        R64FX_TEST_SHIFT_INSTR(int,          SHL, <<) &&
        R64FX_TEST_SHIFT_INSTR(int,          SHR, >>) &&
        R64FX_TEST_SHIFT_INSTR(unsigned int, SAR, >>);
#undef R64FX_LIST2
#undef R64FX_TEST_SHIFT_INSTR
}


bool test_push_pop(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    cout << "PUSH & POP\n";
    {
        long num = rand();
        as.rewindCode();
        as.MOV(r9, Imm64(num));
        as.PUSH(r9);
        as.POP(rax);
        as.RET();
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

    cout << "MOVAPS\n";
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
        as.MOVAPS(xmm0, Mem128(a));
        as.MOVAPS(xmm8, Mem128(b));
        as.MOVAPS(xmm1, xmm8);
        as.MOVAPS(xmm9, xmm0);
        as.MOVAPS(Mem128(c), xmm1);
        as.MOVAPS(Mem128(d), xmm9);
        as.RET();

        jitfun();

        if(!vec4_eq(c, b) || !vec4_eq(a, d))
        {
            return false;
        }
    }

    cout << "MOVUPS\n";
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
        as.MOVUPS(xmm0, Mem32(a));
        as.MOVUPS(xmm8, Mem32(b));
        as.MOVUPS(xmm1, xmm8);
        as.MOVUPS(xmm9, xmm0);
        as.MOVUPS(Mem32(c), xmm1);
        as.MOVUPS(Mem32(d), xmm9);
        as.RET();

        jitfun();

        if(!vec4_eq(c, b) || !vec4_eq(a, d))
        {
            return false;
        }
    }

    cout << "(ADD|SUB|MUL|DIV)PS\n";
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

        as.MOVAPS(xmm0,  Mem128(n));
        as.MOVAPS(xmm1,  Mem128(f));
        as.MOVAPS(xmm2,  Mem128(n));
        as.MOVAPS(xmm3,  Mem128(n));

        as.MOVAPS(xmm8,  Mem128(n));
        as.MOVAPS(xmm9,  Mem128(n));
        as.MOVAPS(xmm10, Mem128(n));
        as.MOVAPS(xmm11, Mem128(n));

        as.MOV(r9, ImmAddr(buff));

        as.ADDPS(xmm8, Mem128(a));
        as.ADDPS(xmm0, Mem128(f));
        as.ADDPS(xmm8, Base(r9) + Disp(4 * 4));
        as.MOVAPS(xmm9, Mem128(a));
        as.ADDPS(xmm8, xmm9);
        as.ADDPS(xmm0, xmm1);

        as.SUBPS(xmm8, Mem128(b));
        as.SUBPS(xmm0, Mem128(f));
        as.SUBPS(xmm8, Base(r9) + Disp(4 * 8));
        as.MOVAPS(xmm9, Mem128(b));
        as.SUBPS(xmm8, xmm9);
        as.SUBPS(xmm0, xmm1);

        as.MULPS(xmm8, Mem128(c));
        as.MULPS(xmm0, Mem128(f));
        as.MULPS(xmm8, Base(r9) + Disp(4 * 12));
        as.MOVAPS(xmm9, Mem128(c));
        as.MULPS(xmm8, xmm9);
        as.MULPS(xmm0, xmm1);

        as.DIVPS(xmm8, Mem128(d));
        as.DIVPS(xmm0, Mem128(f));
        as.DIVPS(xmm8, Base(r9) + Disp(4 * 16));
        as.MOVAPS(xmm9, Mem128(d));
        as.DIVPS(xmm8, xmm9);
        as.DIVPS(xmm0, xmm1);

        as.MOVAPS(Mem128(n), xmm8);
        as.RET();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "P(ADD|SUB)D\n";
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

        as.MOVAPS(xmm0,  Mem128(n));
        as.MOVAPS(xmm1,  Mem128(f));
        as.MOVAPS(xmm2,  Mem128(n));
        as.MOVAPS(xmm3,  Mem128(n));

        as.MOVAPS(xmm8,  Mem128(n));
        as.MOVAPS(xmm9,  Mem128(n));
        as.MOVAPS(xmm10, Mem128(n));
        as.MOVAPS(xmm11, Mem128(n));

        as.MOV(r9, ImmAddr(buff));

        as.PADDD(xmm8, Mem128(a));
        as.PADDD(xmm0, Mem128(f));
        as.PADDD(xmm8, Base(r9) + Disp(4 * 4));
        as.MOVAPS(xmm9, Mem128(a));
        as.PADDD(xmm8, xmm9);
        as.PADDD(xmm0, xmm1);

        as.PSUBD(xmm8, Mem128(b));
        as.PSUBD(xmm0, Mem128(f));
        as.PSUBD(xmm8, Base(r9) + Disp(4 * 8));
        as.MOVAPS(xmm9, Mem128(b));
        as.PSUBD(xmm8, xmm9);
        as.PSUBD(xmm0, xmm1);

        as.MOVAPS(Mem128(n), xmm8);
        as.RET();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "(AND|ANDN|OR|XOR)PS\n";
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

        as.MOVAPS(xmm0,  Mem128(n));
        as.MOVAPS(xmm1,  Mem128(f));
        as.MOVAPS(xmm2,  Mem128(n));
        as.MOVAPS(xmm3,  Mem128(n));

        as.MOVAPS(xmm8,  Mem128(u));
        as.MOVAPS(xmm9,  Mem128(u));
        as.MOVAPS(xmm10, Mem128(u));
        as.MOVAPS(xmm11, Mem128(u));

        as.MOV(r9, ImmAddr(buff));

        as.ANDPS(xmm8, Mem128(a));
        as.PADDD(xmm8, Mem128(u));
        as.ANDPS(xmm0, Mem128(f));
        as.ANDPS(xmm8, Base(r9) + Disp(4 * 4));
        as.PADDD(xmm8, Mem128(u));
        as.MOVAPS(xmm9, Mem128(a));
        as.ANDPS(xmm8, xmm9);
        as.PADDD(xmm8, Mem128(u));
        as.ANDPS(xmm0, xmm1);

        as.ANDNPS(xmm8, Mem128(b));
        as.PADDD(xmm8, Mem128(u));
        as.ANDNPS(xmm0, Mem128(f));
        as.ANDNPS(xmm8, Base(r9) + Disp(4 * 8));
        as.PADDD(xmm8, Mem128(u));
        as.MOVAPS(xmm9, Mem128(b));
        as.ANDNPS(xmm8, xmm9);
        as.PADDD(xmm8, Mem128(u));
        as.ANDNPS(xmm0, xmm1);

        as.ORPS(xmm8, Mem128(c));
        as.PADDD(xmm8, Mem128(u));
        as.ORPS(xmm0, Mem128(f));
        as.ORPS(xmm8, Base(r9) + Disp(4 * 12));
        as.PADDD(xmm8, Mem128(u));
        as.MOVAPS(xmm9, Mem128(c));
        as.ORPS(xmm8, xmm9);
        as.PADDD(xmm8, Mem128(u));
        as.ORPS(xmm0, xmm1);

        as.XORPS(xmm8, Mem128(d));
        as.PADDD(xmm8, Mem128(u));
        as.XORPS(xmm0, Mem128(f));
        as.XORPS(xmm8, Base(r9) + Disp(4 * 16));
        as.PADDD(xmm8, Mem128(u));
        as.MOVAPS(xmm9, Mem128(d));
        as.XORPS(xmm8, xmm9);
        as.PADDD(xmm8, Mem128(u));
        as.XORPS(xmm0, xmm1);

        as.MOVAPS(Mem128(n), xmm8);
        as.RET();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "MINPS\n";
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
        as.MOVAPS(xmm8, Mem128(d));
        as.MINPS(xmm8, Mem128(a));
        as.ADDPS(xmm8, Mem128(m));
        as.MOVAPS(xmm9, Mem128(b));
        as.MINPS(xmm8, xmm9);
        as.MULPS(xmm8, Mem128(m));
        as.MOV(rcx, ImmAddr(buff));
        as.MINPS(xmm8, Base(rcx) + Disp(4 * 8));
        as.MOVAPS(Mem128(n), xmm8);
        as.RET();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "MAXPS\n";
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
        as.MOVAPS(xmm8, Mem128(d));
        as.MAXPS(xmm8, Mem128(a));
        as.ADDPS(xmm8, Mem128(m));
        as.MOVAPS(xmm9, Mem128(b));
        as.MAXPS(xmm8, xmm9);
        as.MULPS(xmm8, Mem128(m));
        as.MOV(rcx, ImmAddr(buff));
        as.MAXPS(xmm8, Base(rcx) + Disp(4 * 8));
        as.MOVAPS(Mem128(n), xmm8);
        as.RET();
        jitfun();

        if(!vec4_eq(r, n))
        {
            return false;
        }
    }

    cout << "PSHUFD\n";
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
        as.PSHUFD(xmm8, Mem128(a1), Shuf(3, 2, 1, 0));
        as.MOVAPS(Mem128(b1), xmm8);

        as.MOVAPS(xmm0, Mem128(a2));
        as.PSHUFD(xmm0, xmm0, Shuf(1, 1, 1, 1));
        as.MOVAPS(Mem128(b2), xmm0);

        as.MOV(rcx, ImmAddr(buff));
        as.PSHUFD(xmm5, Base(rcx) + Disp(4 * 24), Shuf(2, 0, 2, 0));
        as.MOVAPS(Mem128(b3), xmm5);

        as.RET();
        jitfun();

        if(!vec4_eq(b1, c1) || !vec4_eq(b2, c2) || !vec4_eq(b3, c3))
        {
            return false;
        }
    }

    cout << "CMPPLTPS\n";
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

        as.MOVAPS(xmm0, Mem128(a));
        as.MOVAPS(xmm1, Mem128(b));
        as.CMPLTPS(xmm0, xmm1);
        as.MOVAPS(Mem128(c), xmm0);

        as.RET();
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

    cout << "JNZ\n";
    {
        JumpLabel loop, skip;

        as.rewindCode();
        as.MOV(rax, Imm32(0));
        as.MOV(rcx, Imm32(1234));
        as.mark(loop);
        as.ADD(rax, Imm32(2));

        as.JMP(skip);
        as.ADD(rax, Imm32(1));
        as.mark(skip);

        as.SUB(rcx, Imm32(1));
        as.JNZ(loop);
        as.RET();
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

        as.MOV(rax, Imm32(0));
        as.MOV(rcx, ImmAddr(buff));
        as.MOV(rdx, Imm32(10));
        as.MOV(rax, Base(rcx) + Index(rdx) * 8);

        as.RET();
        R64FX_EXPECT_EQ(buff[10], jitfun());
    }

    cout << "[base + index * 8 + Disp8]\n";
    {
        as.rewindData();
        as.growData(16 * sizeof(long));
        auto buff = (long*) as.dataBegin();
        buff[11] = rand() & 0xFFFF;

        as.rewindCode();

        as.MOV(rax, Imm32(0));
        as.MOV(rcx, ImmAddr(buff));
        as.MOV(rdx, Imm32(10));
        as.MOV(rax, Base(rcx) + Index(rdx) * 8 + Disp(8));

        as.RET();
        R64FX_EXPECT_EQ(buff[11], jitfun());
    }

    cout << "[base + index * 8 + Disp32]\n";
    {
        as.rewindData();
        as.growData((128) * sizeof(long));
        auto buff = (long*) as.dataBegin();
        buff[10 + 64] = rand() & 0xFFFF;

        as.rewindCode();

        as.MOV(rax, Imm32(0));
        as.MOV(rcx, ImmAddr(buff));
        as.MOV(rdx, Imm32(10));
        as.MOV(rax, Base(rcx) + Index(rdx) * 8 + Disp(64 * 8));

        as.RET();
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

        as.MOV(rcx, ImmAddr(buff));
        as.MOV(rdx, Imm32(4));
        as.MOVAPS(xmm0, Base(rcx) + Index(rdx) * 4);
        as.MOVAPS(xmm8, Base(rcx) + Index(rdx) * 8);
        as.MOVAPS(Base(rcx), xmm0);

        as.RET();
        jitfun();

        if(!vec4_eq(buff, buff + 4))
        {
            return false;
        }
    }

    cout << "[index + disp8]\n";
    {
        as.rewindData();
        as.growData(sizeof(long) * 8);
        auto buff = (long*)as.dataBegin();
        for(int i=0; i<8; i++)
            buff[i] = long(i);

        as.rewindCode();

        as.MOV(rcx, ImmAddr(buff));
        as.SAR(rcx, Imm8(1));
        as.MOV(rax, Index(rcx)*2 + Disp(8));
        as.SAR(rcx, Imm8(1));
        as.MOV(Index(rcx)*4 + Disp(16), rax);

        as.RET();
        jitfun();

        R64FX_EXPECT_EQ(buff[1], buff[2]);
    }

    {
        as.rewindData();
        as.growData(sizeof(int) * 8);
        auto buff = (int*)as.dataBegin();
        for(int i=0; i<8; i++)
            buff[i] = rand() & 0xFFFFFFFF;

        cout << "MOV(GPR32, Base)\n";
        as.rewindCode();
        as.MOV(rax, ImmAddr(buff + 3));
        as.MOV(eax, Base(rax));
        as.RET();
        R64FX_EXPECT_EQ(buff[3], jitfun());

        cout << "MOV(GPR32, Base) + rex 1\n";
        as.rewindCode();
        as.MOV(r9, ImmAddr(buff + 3));
        as.MOV(eax, Base(r9));
        as.RET();
        R64FX_EXPECT_EQ(buff[3], jitfun());

        cout << "MOV(GPR32, Base) + rex 2\n";
        as.rewindCode();
        as.MOV(r9, ImmAddr(buff + 3));
        as.MOV(r9d, Base(r9));
        as.MOV(rax, r9);
        as.RET();
        R64FX_EXPECT_EQ(buff[3], jitfun());

        cout << "MOV(Base, GPR32)\n";
        as.rewindCode();
        as.MOV(rax, Imm32(1234567));
        as.MOV(rdx, ImmAddr(buff + 3));
        as.MOV(Base(rdx), rax);
        as.RET();
        jitfun();
        R64FX_EXPECT_EQ(1234567, buff[3]);

        cout << "MOV(Base, GPR32) + rex\n";
        as.rewindCode();
        as.MOV(rax, Imm32(555666));
        as.MOV(r8, Imm32(121213));
        as.MOV(rdx, ImmAddr(buff + 3));
        as.MOV(Base(rdx), r8);
        as.RET();
        jitfun();
        R64FX_EXPECT_EQ(121213, buff[3]);
    }

    cout << "\n";
    return true;
}


bool test_movdq(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();
    as.rewindData();
    as.growData(sizeof(float) * 16);
    auto buff = (float*) as.dataBegin();

    cout << "MOV(D|Q)\n";
    {
        for(int i=0; i<8; i++)
            buff[i] = float(rand() & 0xFFFF) * 0.01;
        for(int i=8; i<16   ; i++)
            buff[i] = 0.0f;

        as.rewindCode();
        as.MOVAPS(xmm0, Mem128(buff));
        as.MOVAPS(xmm8, Mem128(buff + 4));
        as.MOVD(eax, xmm0);
        as.MOVQ(rcx, xmm8);
        as.MOVD(xmm8, eax);
        as.MOVQ(xmm0, rcx);
        as.MOVAPS(Mem128(buff + 8), xmm8);
        as.MOVAPS(Mem128(buff + 12), xmm0);
        as.RET();
        jitfun();

        buff[1] = buff[2] = buff[3] = buff[6] = buff[7] = 0.0f;

        if(!vec4_eq(buff, buff + 8) || !vec4_eq(buff + 4, buff + 12))
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
        test_gpr_instrs(as) &&
        test_shift_instrs(as) &&
        test_push_pop(as) &&
        test_sse(as) &&
        test_jumps(as) &&
        test_sibd(as) &&
        test_movdq(as)
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
