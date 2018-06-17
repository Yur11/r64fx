/* Test overall operation.
 *
 * Test REX prefix. Incorrect prefix order may cause cpu to ignore REX prefix altogether
 * thus encoding a wrong register.
 * Checking for rax being encoded instead r8, rcx instead of r9 etc...
 */

#include "test.hpp"
#include "jit.hpp"
#include <iostream>

#define R64FX_LIST0(a)
#define R64FX_LIST1(a) a
#define R64FX_LIST2(a) a,a
#define R64FX_LIST3(a) a,a,a
#define R64FX_LIST4(a) a,a,a,a
#define R64FX_LIST5(a) a,a,a,a,a
#define R64FX_LIST6(a) a,a,a,a,a,a
#define R64FX_LIST7(a) a,a,a,a,a,a,a
#define R64FX_LIST8(a) a,a,a,a,a,a,a,a
#define R64FX_LIST9(a) a,a,a,a,a,a,a,a,a

#define R64FX_LIST(a,N) R64FX_LIST##N(a)

using namespace std;
using namespace r64fx;

template<typename T> inline void dump(T* v, unsigned long size)
{
    for(unsigned long i=0; i<size; i++)
        cout << v[i] << (i == (size-1) ? "\n" : ", ");
}


bool test_buffers(Assembler &as)
{
//     constexpr unsigned char* null_ptr = nullptr;
// 
//     as.resize(1, 1);
//     as.resize(1, 0);
//     as.resize(0, 1);
//     as.resize(1, 1);
//     R64FX_EXPECT_EQ(true, as.dataEnd() == as.begin());
//     as.resize(0, 0);
//     R64FX_EXPECT_EQ(0UL, as.dataPageCount());
//     R64FX_EXPECT_EQ(0UL, as.codePageCount());
//     R64FX_EXPECT_EQ(null_ptr, as.dataBegin());
//     R64FX_EXPECT_EQ(null_ptr, as.begin());
//     R64FX_EXPECT_EQ(null_ptr, as.codeEnd());
//     as.NOP(1);
//     R64FX_EXPECT_EQ(0UL, as.dataPageCount());
//     R64FX_EXPECT_EQ(1UL, as.codePageCount());
//     R64FX_EXPECT_EQ(as.dataEnd() - 5678, as.growData(5678));
//     R64FX_EXPECT_EQ((unsigned long)(5678 / memory_page_size() + 1), as.dataPageCount());
// 
//     /* Check code & data relocations! */
//     as.rewindData();
//     R64FX_EXPECT_EQ(long(as.dataEnd()), long(as.begin()));
//     as.growData(16);
//     R64FX_EXPECT_EQ(long(as.dataEnd()), long(as.begin()));
//     ((long*)(as.dataBegin()))[0] = 0x2AAAAAAAAAAAAAAAL;
//     ((long*)(as.dataBegin()))[1] = 0x5555555555555555L;
//     as.rewind();
// 
//     //Add 1 to 0x2AAAAAAAAAAAAAAAL
//     as.MOV(rax, Imm32(1));
//     as.ADD(Mem64(as.dataBegin()), rax);
//     as.NOP(memory_page_size() + (rand() & 0xF));
// 
//     as.MOV(rdx, Imm32(100));
//     as.NOP(memory_page_size() + (rand() & 0xF));
// 
//     as.MOV(rcx, Imm32(10));
//     as.NOP(memory_page_size() + (rand() & 0xF));
// 
//     //Sub 16 from 0x5555555555555555L
//     as.MOV(rax, Imm32(16));
//     as.SUB(Mem64(as.dataBegin() + 8), rax);
//     as.NOP(memory_page_size() + (rand() & 0xF));
// 
//     as.MOV(rax, Imm32(1));
//     as.NOP(memory_page_size() + (rand() & 0xF));
// 
//     as.ADD(rax, rcx);
//     as.NOP(memory_page_size() + (rand() & 0xF));
//     as.ADD(rax, rdx);
//     as.RET();
// 
//     auto fun = (long(*)()) as.begin();
//     R64FX_EXPECT_EQ(111, fun());
//     R64FX_EXPECT_EQ(0x2AAAAAAAAAAAAAABL, ((long*)(as.dataBegin()))[0]); // +1
//     R64FX_EXPECT_EQ(0x5555555555555545L, ((long*)(as.dataBegin()))[1]); // -16
// 
//     as.resize(1, 1);
//     cout << "\n";
    return true;
}


bool test_lea(Assembler &as)
{
    auto fun = (long(*)()) as.begin();

    cout << "LEA RIP\n";
    as.rewind();
    as.LEA(rax, Mem8(as.begin()));
    as.RET();
    R64FX_EXPECT_EQ((long)as.begin(), fun());

    cout << "LEA SIB\n";
    as.rewind();
    as.MOV(rcx, Imm32(123));
    as.MOV(rdx, Imm32(1));
    as.LEA(rax, Base(rcx) + Index(rdx)*8 + Disp(-1));
    as.RET();
    R64FX_EXPECT_EQ(130, fun());

    cout << "\n";
    return true;
}


// bool test_mov(Assembler &as)
// {
//     auto fun = (long(*)()) as.begin();
// 
//     as.rewindData();
//     as.growData(2 * sizeof(long));
//     auto buff = (long*) as.dataBegin();
//     auto a = buff;
//     auto b = buff + 1;
// 
//     cout << "MOV(GPR32, Imm32)\n";
//     {
//         int num = rand();
//         as.rewind();
//         as.MOV(eax, Imm32(num));
//         as.RET();
//         R64FX_EXPECT_EQ(num, fun());
//     }
// 
//     cout << "MOV(GPR64, Imm32)\n";
//     {
//         int num = rand();
//         as.rewind();
//         as.MOV(rax, Imm32(num));
//         as.RET();
//         R64FX_EXPECT_EQ(num, fun());
//     }
// 
//     cout << "MOV(GPR64, Imm32) rex\n";
//     {
//         int num1 = rand();
//         int num2 = rand();
//         as.rewind();
//         as.MOV(rax, Imm32(num1));
//         as.MOV(r8,  Imm32(num2));
//         as.RET();
//         R64FX_EXPECT_EQ(num1, fun())
//     }
// 
//     cout << "MOV(GPR64, Imm64)\n";
//     {
//         long int num = rand();
//         as.rewind();
//         as.MOV(rax, Imm64(num));
//         as.RET();
//         R64FX_EXPECT_EQ(num, fun());
//     }
// 
//     cout << "MOV(GPR64, Imm64) rex\n";
//     {
//         int num1 = rand();
//         int num2 = rand();
//         as.rewind();
//         as.MOV(rax, Imm64(num1));
//         as.MOV(r8,  Imm64(num2));
//         as.RET();
//         R64FX_EXPECT_EQ(num1, fun());
//     }
// 
//     cout << "MOV(GPR64, GPR64)\n";
//     {
//         int num = rand();
//         as.rewind();
//         as.MOV(rcx, Imm32(num));
//         as.MOV(rax, rcx);
//         as.RET();
//         R64FX_EXPECT_EQ(num, fun());
//     }
// 
//     cout << "MOV(GPR64, GPR64) rex\n";
//     {
//         int num1 = rand();
//         int num2 = rand();
//         int num3 = rand();
//         as.rewind();
//         as.MOV(r9,  Imm32(num1));
//         as.MOV(rdx, Imm32(num2));
//         as.MOV(rcx, Imm32(num3));
//         as.MOV(rax, r9);
//         as.MOV(r8,  rdx);
//         as.RET();
//         R64FX_EXPECT_EQ(num1, fun());
//     }
// 
//     cout << "MOV(GPR64, Mem64)\n";
//     {
//         *a = rand();
//         as.rewind();
//         as.MOV(rax, Mem64(a));
//         as.RET();
//         R64FX_EXPECT_EQ(*a, fun());
//     }
// 
//     cout << "MOV(GPR32, Mem32)\n";
//     {
//         auto aa = (int*)a;
//         auto a1 = aa;
//         auto a2 = aa + 1;
//         *a1 = rand() & 0xFFFFFF;
//         *a2 = rand() & 0xFFFFFF;
//         as.rewind();
//         as.MOV(eax, Mem32(a2));
//         as.MOV(r8d, Mem32(a1));
//         as.RET();
//         R64FX_EXPECT_EQ(*a2, fun());
//     }
// 
//     cout << "MOV(GPR64, Mem64) rex\n";
//     {
//         *a = rand();
//         *b = rand();
//         as.rewind();
//         as.MOV(rax, Mem64(a));
//         as.MOV(r8,  Mem64(b));
//         as.RET();
//         R64FX_EXPECT_EQ(*a, fun());
//     }
// 
//     cout << "MOV(Mem64, GPR64)\n";
//     {
//         int num = rand();
//         *a = rand();
//         as.rewind();
//         as.MOV(rcx, Imm32(num));
//         as.MOV(Mem64(a), rcx);
//         as.RET();
//         fun();
//         R64FX_EXPECT_EQ(num, *a);
//     }
// 
//     cout << "MOV(Mem64, GPR64) rex\n";
//     {
//         int num1 = rand();
//         int num2 = rand();
//         *a = 0;
//         as.rewind();
//         as.MOV(rax, Imm32(num1));
//         as.MOV(r8,  Imm32(num2));
//         as.MOV(Mem64(a), r8);
//         as.RET();
//         fun();
//         R64FX_EXPECT_EQ(num2, *a);
//     }
// 
//     {
//         as.rewindData();
//         as.growData(sizeof(int) * 4);
//         auto ptr = (int*) as.dataBegin();
// 
//         cout << "MOV(GPR32, Mem32)\n";
//         for(int i=0; i<4; i++)
//             ptr[i] = rand();
//         as.rewind();
//         as.MOV(eax, Mem32(ptr + 1));
//         as.RET();
//         R64FX_EXPECT_EQ(ptr[1], fun());
// 
//         cout << "MOV(GPR32, Mem32) rex\n";
//         for(int i=0; i<4; i++)
//             ptr[i] = rand();
//         as.rewind();
//         as.MOV(eax, Mem32(ptr + 1));
//         as.MOV(r8d, Mem32(ptr + 2));
//         as.RET();
//         R64FX_EXPECT_EQ(ptr[1], fun());
// 
//         cout << "MOV(Mem32, GPR)\n";
//         for(int i=0; i<4; i++)
//             ptr[i] = rand();
//         as.rewind();
//         as.MOV(eax, Imm32(ptr[0]));
//         as.MOV(Mem32(ptr + 1), eax);
//         as.RET();
//         fun();
//         R64FX_EXPECT_EQ(ptr[0], ptr[1]);
//     }
// 
//     cout << "\n";
//     return true;
// }


// bool test_cmov(Assembler &as)
// {
//     auto fun = (long(*)(long, long)) as.begin();
// 
//     cout << "CMOVZ(GPR64, GPR64)\n";
//     as.rewind();
//     as.MOV   (rax, rdi);
//     as.XOR   (rcx, rcx);
//     as.ADD   (rcx, Imm32(1));
//     as.XOR   (rcx, rcx);
//     as.CMOVZ (rax, rsi);
//     as.RET   ();
//     R64FX_EXPECT_EQ(456, fun(123, 456));
// 
//     cout << "\n";
//     return true;
// }


// template<
//     void (Assembler::*gpr64_imm32) (GPR64, Imm32),
//     void (Assembler::*gpr64_imm8 ) (GPR64, Imm8 ),
//     void (Assembler::*gpr64_gpr64) (GPR64, GPR64),
//     void (Assembler::*gpr64_mem64) (GPR64, Mem64),
//     void (Assembler::*gpr64_sibd ) (GPR64, SIBD ),
//     void (Assembler::*mem64_gpr64) (Mem64, GPR64),
//     void (Assembler::* sibd_gpr64) (SIBD,  GPR64)
// > bool test_gpr_inst(const char* name, Assembler &as, int (*expected)(int a, int b))
// {
//     auto jitfun = (long(*)()) as.begin();
// 
//     as.rewindData();
//     as.growData(4 * sizeof(long));
// 
//     cout << name << "(GPR64, Imm32)\n";
//     {
//         int a = rand() & 0xFFFFFF;
//         int b = rand() & 0xFFFFFF;
//         int c = rand() & 0xFFFFFF;
// 
//         as.rewind();
//         as.MOV(rax, Imm32(a));
//         (as.*gpr64_imm32)(rax, Imm32(b));
//         (as.*gpr64_imm32)(r8,  Imm32(c));
//         as.RET();
//         R64FX_EXPECT_EQ(expected(a, b), jitfun());
//     }
// 
//     cout << name << "(GPR64, Imm8 )\n";
//     {
//         char a = rand() & 0xFF;
//         char b = rand() & 0xFF;
//         char c = rand() & 0xFF;
// 
//         as.rewind();
//         as.MOV(rax, Imm32(a));
//         (as.*gpr64_imm8)(rax, Imm8(b));
//         (as.*gpr64_imm8)(r8,  Imm8(c));
//         as.RET();
//         R64FX_EXPECT_EQ(expected(a, b), jitfun());
//     }
// 
//     cout << name << "(GPR64, GPR64)\n";
//     {
//         int a = rand() & 0xFFFFFF;
//         int b = rand() & 0xFFFFFF;
//         int c = rand() & 0xFFFFFF;
// 
//         as.rewind();
//         as.MOV(rax, Imm32(a));
//         as.MOV(rcx, Imm32(b));
//         as.MOV(rdx, Imm32(c));
//         (as.*gpr64_gpr64)(rax, rcx);
//         (as.*gpr64_gpr64)(r8,  rdx);
//         as.RET();
//         R64FX_EXPECT_EQ(expected(a, b), jitfun());
//     }
// 
//     cout << name << "(GPR64, Mem64)\n";
//     {
//         auto buff = (long*) as.dataBegin();
//         for(int i=0; i<4; i++)
//         {
//             buff[i] = rand() & 0xFFFFFF;
//         }
// 
//         as.rewind();
//         as.MOV(rax, Imm32(0));
//         as.MOV(rax, Mem64(buff + 0));
//         (as.*gpr64_mem64)(rax, Mem64(buff + 1));
//         (as.*gpr64_mem64)(r8,  Mem64(buff + 2));
//         as.RET();
//         R64FX_EXPECT_EQ(expected(buff[0], buff[1]), jitfun());
//     }
// 
//     cout << name << "(GPR64, SIBD )\n";
//     {
//         auto buff = (long*) as.dataBegin();
//         for(int i=0; i<4; i++)
//         {
//             buff[i] = rand() & 0xFFFFFF;
//         }
// 
//         as.rewind();
//         as.MOV(rax, Imm32(0));
//         as.MOV(r8,  Imm32(1));
//         as.MOV(rax, Mem64(buff + 0));
//         as.MOV(rdx, ImmAddr(buff + 1));
//         as.MOV(rcx, Imm32(1));
//         (as.*gpr64_sibd)(rax, Base(rdx) + Index(rcx) * 8 + Disp(8));
//         (as.*gpr64_sibd)(r8, Base(rdx) + Index(rcx) * 8 + Disp(8));
//         as.RET();
//         R64FX_EXPECT_EQ(expected(buff[0], buff[3]), jitfun());
//     }
// 
//     cout << "\n";
//     return true;
// }


// bool test_gpr_instrs(Assembler &as)
// {
// #define R64FX_LIST7(a) a,a,a,a,a,a,a
// #define R64FX_TEST_GPR_INSTR(name, op) test_gpr_inst<R64FX_LIST7(&Assembler::name)>(#name, as, [](int a, int b){ return a op b; })
//     return
//         R64FX_TEST_GPR_INSTR(ADD, +) &&
//         R64FX_TEST_GPR_INSTR(SUB, -) &&
//         R64FX_TEST_GPR_INSTR(XOR, ^) &&
//         R64FX_TEST_GPR_INSTR(AND, &) &&
//         R64FX_TEST_GPR_INSTR(OR,  |);
// #undef R64FX_LIST7
// #undef R64FX_TEST_GPR_INSTR
// }


// template<
//     typename IntT,
//     void (Assembler::*shift1)     (GPR64 gpr),
//     void (Assembler::*shift_imm8) (GPR64 gpr, Imm8)
// >
// bool test_shift_instr(const char* name, Assembler &as, IntT (*expected)(IntT num, IntT shift))
// {
//     auto jitfun = (long(*)()) as.begin();
// 
//     cout << name << "(GPR, 1)\n";
//     {
//         IntT num = rand() & 0xFFFF;
//         as.rewind();
//         as.MOV(rax, Imm32(num));
//         (as.*shift1)(rax);
//         (as.*shift1)(r8);
//         as.RET();
//         R64FX_EXPECT_EQ(expected(num, 1), jitfun());
//     }
// 
//     cout << name << "(GPR, Imm8)\n";
//     {
//         IntT num = rand() & 0xFF;
//         IntT shift = ((IntT)(rand() & 0x3)) + 1;
//         as.rewind();
//         as.MOV(rax, Imm32(num));
//         (as.*shift_imm8)(rax, Imm8(shift));
//         (as.*shift_imm8)(r8,  Imm8(shift));
//         as.RET();
//         R64FX_EXPECT_EQ(expected(num, shift), jitfun());
//     }
// 
//     return true;
// }
// 
// 
// bool test_shift_instrs(Assembler &as)
// {
// #define R64FX_LIST2(a) a,a
// #define R64FX_TEST_SHIFT_INSTR(type, name, op) test_shift_instr<type, R64FX_LIST2(&Assembler::name)>(#name, as, [](type num, type shift){ return num op shift; })
//     return
//         R64FX_TEST_SHIFT_INSTR(int,          SHL, <<) &&
//         R64FX_TEST_SHIFT_INSTR(int,          SHR, >>) &&
//         R64FX_TEST_SHIFT_INSTR(unsigned int, SAR, >>);
// #undef R64FX_LIST2
// #undef R64FX_TEST_SHIFT_INSTR
// }
// 
// 
// bool test_push_pop(Assembler &as)
// {
//     auto jitfun = (long(*)()) as.begin();
// 
//     cout << "PUSH & POP\n";
//     {
//         long num = rand();
//         as.rewind();
//         as.MOV(r9, Imm64(num));
//         as.PUSH(r9);
//         as.POP(rax);
//         as.RET();
//         R64FX_EXPECT_EQ(num, jitfun());
//     }
// 
//     cout << "\n";
//     return true;
// }


// bool test_sse(Assembler &as)
// {
//     auto jitfun = (long(*)()) as.begin();
//     as.rewindData();
//     as.growData(64 * sizeof(float));
// 

// 
//     cout << "(ADD|SUB|MUL|DIV)PS\n";
//     {
//         float* buff = (float*) as.dataBegin();
//         auto a = buff + 4;
//         auto b = buff + 8;
//         auto c = buff + 12;
//         auto d = buff + 16;
//         auto f = buff + 20;
//         auto r = buff + 24;
//         auto n = buff + 28;
// 
//         for(int i=0; i<4; i++)
//         {
//             n[i] = 0.0f;
//             a[i] = float(rand());
//             b[i] = float(rand());
//             c[i] = float(rand());
//             do{ d[i] = float(rand()); } while(d[i] == 0.0f);
//             r[i] = ((((a[i] + a[i] + a[i] - b[i] - b[i] - b[i]) * c[i] * c[i] * c[i]) / d[i]) / d[i]) / d[i];
//             f[i] = float(rand());
//         }
// 
//         as.rewind();
// 
//         as.MOVAPS(xmm0,  Mem128(n));
//         as.MOVAPS(xmm1,  Mem128(f));
//         as.MOVAPS(xmm2,  Mem128(n));
//         as.MOVAPS(xmm3,  Mem128(n));
// 
//         as.MOVAPS(xmm8,  Mem128(n));
//         as.MOVAPS(xmm9,  Mem128(n));
//         as.MOVAPS(xmm10, Mem128(n));
//         as.MOVAPS(xmm11, Mem128(n));
// 
//         as.MOV(r9, ImmAddr(buff));
// 
//         as.ADDPS(xmm8, Mem128(a));
//         as.ADDPS(xmm0, Mem128(f));
//         as.ADDPS(xmm8, Base(r9) + Disp(4 * 4));
//         as.MOVAPS(xmm9, Mem128(a));
//         as.ADDPS(xmm8, xmm9);
//         as.ADDPS(xmm0, xmm1);
// 
//         as.SUBPS(xmm8, Mem128(b));
//         as.SUBPS(xmm0, Mem128(f));
//         as.SUBPS(xmm8, Base(r9) + Disp(4 * 8));
//         as.MOVAPS(xmm9, Mem128(b));
//         as.SUBPS(xmm8, xmm9);
//         as.SUBPS(xmm0, xmm1);
// 
//         as.MULPS(xmm8, Mem128(c));
//         as.MULPS(xmm0, Mem128(f));
//         as.MULPS(xmm8, Base(r9) + Disp(4 * 12));
//         as.MOVAPS(xmm9, Mem128(c));
//         as.MULPS(xmm8, xmm9);
//         as.MULPS(xmm0, xmm1);
// 
//         as.DIVPS(xmm8, Mem128(d));
//         as.DIVPS(xmm0, Mem128(f));
//         as.DIVPS(xmm8, Base(r9) + Disp(4 * 16));
//         as.MOVAPS(xmm9, Mem128(d));
//         as.DIVPS(xmm8, xmm9);
//         as.DIVPS(xmm0, xmm1);
// 
//         as.MOVAPS(Mem128(n), xmm8);
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(r, n, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "P(ADD|SUB)D\n";
//     {
//         int* buff = (int*) as.dataBegin();
//         auto a = buff + 4;
//         auto b = buff + 8;
//         auto f = buff + 20;
//         auto r = buff + 24;
//         auto n = buff + 28;
// 
//         for(int i=0; i<4; i++)
//         {
//             n[i] = 0.0f;
//             a[i] = rand();
//             b[i] = rand();
//             r[i] = a[i] + a[i] + a[i] - b[i] - b[i] - b[i];
//             f[i] = rand();
//         }
// 
//         as.rewind();
// 
//         as.MOVAPS(xmm0,  Mem128(n));
//         as.MOVAPS(xmm1,  Mem128(f));
//         as.MOVAPS(xmm2,  Mem128(n));
//         as.MOVAPS(xmm3,  Mem128(n));
// 
//         as.MOVAPS(xmm8,  Mem128(n));
//         as.MOVAPS(xmm9,  Mem128(n));
//         as.MOVAPS(xmm10, Mem128(n));
//         as.MOVAPS(xmm11, Mem128(n));
// 
//         as.MOV(r9, ImmAddr(buff));
// 
//         as.PADDD(xmm8, Mem128(a));
//         as.PADDD(xmm0, Mem128(f));
//         as.PADDD(xmm8, Base(r9) + Disp(4 * 4));
//         as.MOVAPS(xmm9, Mem128(a));
//         as.PADDD(xmm8, xmm9);
//         as.PADDD(xmm0, xmm1);
// 
//         as.PSUBD(xmm8, Mem128(b));
//         as.PSUBD(xmm0, Mem128(f));
//         as.PSUBD(xmm8, Base(r9) + Disp(4 * 8));
//         as.MOVAPS(xmm9, Mem128(b));
//         as.PSUBD(xmm8, xmm9);
//         as.PSUBD(xmm0, xmm1);
// 
//         as.MOVAPS(Mem128(n), xmm8);
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(r, n, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "(AND|ANDN|OR|XOR)PS\n";
//     {
//         int* buff = (int*) as.dataBegin();
//         auto a = buff + 4;
//         auto b = buff + 8;
//         auto c = buff + 12;
//         auto d = buff + 16;
//         auto f = buff + 20;
//         auto r = buff + 24;
//         auto n = buff + 28;
//         auto u = buff + 32;
// 
//         int num = 1 + 4 + 16 + 32 + 128 + 1024 + 2048 + 8196;
//         for(int i=0; i<4; i++)
//         {
//             n[i] = 0;
//             u[i] = num;
//             a[i] = rand();
//             b[i] = rand();
//             c[i] = rand();
//             d[i] = rand();
// 
//             r[i] = u[i];
//             r[i] = r[i] & a[i];
//             r[i] += num;
//             r[i] = r[i] & a[i];
//             r[i] += num;
//             r[i] = r[i] & a[i];
//             r[i] += num;
//             r[i] = (~r[i]) & b[i];
//             r[i] += num;
//             r[i] = (~r[i]) & b[i];
//             r[i] += num;
//             r[i] = (~r[i]) & b[i];
//             r[i] += num;
//             r[i] = r[i] | c[i];
//             r[i] += num;
//             r[i] = r[i] | c[i];
//             r[i] += num;
//             r[i] = r[i] | c[i];
//             r[i] += num;
//             r[i] = r[i] ^ d[i];
//             r[i] += num;
//             r[i] = r[i] ^ d[i];
//             r[i] += num;
//             r[i] = r[i] ^ d[i];
//             r[i] += num;
// 
//             f[i] = rand();
//         }
// 
//         as.rewind();
// 
//         as.MOVAPS(xmm0,  Mem128(n));
//         as.MOVAPS(xmm1,  Mem128(f));
//         as.MOVAPS(xmm2,  Mem128(n));
//         as.MOVAPS(xmm3,  Mem128(n));
// 
//         as.MOVAPS(xmm8,  Mem128(u));
//         as.MOVAPS(xmm9,  Mem128(u));
//         as.MOVAPS(xmm10, Mem128(u));
//         as.MOVAPS(xmm11, Mem128(u));
// 
//         as.MOV(r9, ImmAddr(buff));
// 
//         as.ANDPS(xmm8, Mem128(a));
//         as.PADDD(xmm8, Mem128(u));
//         as.ANDPS(xmm0, Mem128(f));
//         as.ANDPS(xmm8, Base(r9) + Disp(4 * 4));
//         as.PADDD(xmm8, Mem128(u));
//         as.MOVAPS(xmm9, Mem128(a));
//         as.ANDPS(xmm8, xmm9);
//         as.PADDD(xmm8, Mem128(u));
//         as.ANDPS(xmm0, xmm1);
// 
//         as.ANDNPS(xmm8, Mem128(b));
//         as.PADDD(xmm8, Mem128(u));
//         as.ANDNPS(xmm0, Mem128(f));
//         as.ANDNPS(xmm8, Base(r9) + Disp(4 * 8));
//         as.PADDD(xmm8, Mem128(u));
//         as.MOVAPS(xmm9, Mem128(b));
//         as.ANDNPS(xmm8, xmm9);
//         as.PADDD(xmm8, Mem128(u));
//         as.ANDNPS(xmm0, xmm1);
// 
//         as.ORPS(xmm8, Mem128(c));
//         as.PADDD(xmm8, Mem128(u));
//         as.ORPS(xmm0, Mem128(f));
//         as.ORPS(xmm8, Base(r9) + Disp(4 * 12));
//         as.PADDD(xmm8, Mem128(u));
//         as.MOVAPS(xmm9, Mem128(c));
//         as.ORPS(xmm8, xmm9);
//         as.PADDD(xmm8, Mem128(u));
//         as.ORPS(xmm0, xmm1);
// 
//         as.XORPS(xmm8, Mem128(d));
//         as.PADDD(xmm8, Mem128(u));
//         as.XORPS(xmm0, Mem128(f));
//         as.XORPS(xmm8, Base(r9) + Disp(4 * 16));
//         as.PADDD(xmm8, Mem128(u));
//         as.MOVAPS(xmm9, Mem128(d));
//         as.XORPS(xmm8, xmm9);
//         as.PADDD(xmm8, Mem128(u));
//         as.XORPS(xmm0, xmm1);
// 
//         as.MOVAPS(Mem128(n), xmm8);
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(r, n, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "MINPS\n";
//     {
//         float* buff = (float*) as.dataBegin();
//         auto a = buff;
//         auto b = buff + 4;
//         auto c = buff + 8;
//         auto d = buff + 12;
//         auto n = buff + 16;
//         auto r = buff + 20;
//         auto m = buff + 24;
// 
//         for(int i=0; i<4; i++)
//         {
//             m[i] = float(rand());
//             a[i] = float(rand());
//             b[i] = float(rand());
//             c[i] = float(rand());
//             d[i] = float(rand());
//             r[i] = d[i];
//             r[i] = (r[i] < a[i] ? r[i] : a[i]);
//             r[i] += m[i];
//             r[i] = (r[i] < b[i] ? r[i] : b[i]);
//             r[i] *= m[i];
//             r[i] = (r[i] < c[i] ? r[i] : c[i]);
//             n[i] = 0;
//         }
// 
//         as.rewind();
//         as.MOVAPS(xmm8, Mem128(d));
//         as.MINPS(xmm8, Mem128(a));
//         as.ADDPS(xmm8, Mem128(m));
//         as.MOVAPS(xmm9, Mem128(b));
//         as.MINPS(xmm8, xmm9);
//         as.MULPS(xmm8, Mem128(m));
//         as.MOV(rcx, ImmAddr(buff));
//         as.MINPS(xmm8, Base(rcx) + Disp(4 * 8));
//         as.MOVAPS(Mem128(n), xmm8);
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(r, n, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "MAXPS\n";
//     {
//         float* buff = (float*) as.dataBegin();
//         auto a = buff;
//         auto b = buff + 4;
//         auto c = buff + 8;
//         auto d = buff + 12;
//         auto n = buff + 16;
//         auto r = buff + 20;
//         auto m = buff + 24;
// 
//         for(int i=0; i<4; i++)
//         {
//             m[i] = float(rand());
//             a[i] = float(rand());
//             b[i] = float(rand());
//             c[i] = float(rand());
//             d[i] = float(rand());
//             r[i] = d[i];
//             r[i] = (r[i] > a[i] ? r[i] : a[i]);
//             r[i] += m[i];
//             r[i] = (r[i] > b[i] ? r[i] : b[i]);
//             r[i] *= m[i];
//             r[i] = (r[i] > c[i] ? r[i] : c[i]);
//             n[i] = 0;
//         }
// 
//         as.rewind();
//         as.MOVAPS(xmm8, Mem128(d));
//         as.MAXPS(xmm8, Mem128(a));
//         as.ADDPS(xmm8, Mem128(m));
//         as.MOVAPS(xmm9, Mem128(b));
//         as.MAXPS(xmm8, xmm9);
//         as.MULPS(xmm8, Mem128(m));
//         as.MOV(rcx, ImmAddr(buff));
//         as.MAXPS(xmm8, Base(rcx) + Disp(4 * 8));
//         as.MOVAPS(Mem128(n), xmm8);
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(r, n, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "PSHUFD\n";
//     {
//         int* buff = (int*) as.dataBegin();
//         auto a1 = buff;
//         auto b1 = buff + 4;
//         auto c1 = buff + 8;
//         auto a2 = buff + 12;
//         auto b2 = buff + 16;
//         auto c2 = buff + 20;
//         auto a3 = buff + 24;
//         auto b3 = buff + 28;
//         auto c3 = buff + 32;
// 
//         for(int i=0; i<4; i++)
//         {
//             a1[i] = rand();
//             b1[i] = 0;
// 
//             a2[i] = rand();
//             b2[i] = 0;
// 
//             a3[i] = rand();
//             b3[i] = 0;
//         }
// 
//         c1[3] = a1[0];
//         c1[2] = a1[1];
//         c1[1] = a1[2];
//         c1[0] = a1[3];
// 
//         c2[3] = a2[1];
//         c2[2] = a2[1];
//         c2[1] = a2[1];
//         c2[0] = a2[1];
// 
//         c3[3] = a3[0];
//         c3[2] = a3[2];
//         c3[1] = a3[0];
//         c3[0] = a3[2];
// 
//         as.rewind();
//         as.PSHUFD(xmm8, Mem128(a1), Shuf(3, 2, 1, 0));
//         as.MOVAPS(Mem128(b1), xmm8);
// 
//         as.MOVAPS(xmm0, Mem128(a2));
//         as.PSHUFD(xmm0, xmm0, Shuf(1, 1, 1, 1));
//         as.MOVAPS(Mem128(b2), xmm0);
// 
//         as.MOV(rcx, ImmAddr(buff));
//         as.PSHUFD(xmm5, Base(rcx) + Disp(4 * 24), Shuf(2, 0, 2, 0));
//         as.MOVAPS(Mem128(b3), xmm5);
// 
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(b1, c1, 4) || !vec_eq(b2, c2, 4) || !vec_eq(b3, c3, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "CMPPLTPS\n";
//     {
//         float* buff = (float*) as.dataBegin();
//         auto a = buff;
//         auto b = buff + 4;
//         auto c = (int*)(buff + 8);
//         auto d = (int*)(buff + 12);
//         for(int i=0; i<4; i++)
//         {
//             a[i] = float(rand() & 0xFF) * 0.5f;
//             b[i] = float(rand() & 0xFF) * 0.5f;
//             d[i] = (a[i] < b[i] ? -1 : 0);
//         }
// 
//         as.rewind();
// 
//         as.MOVAPS(xmm0, Mem128(a));
//         as.MOVAPS(xmm1, Mem128(b));
//         as.CMPLTPS(xmm0, xmm1);
//         as.MOVAPS(Mem128(c), xmm0);
// 
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(c, d, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "\n";
//     return true;
// }


// bool test_jumps(Assembler &as)
// {
//     auto fun = (long(*)(unsigned char*)) as.begin();
// 
//     cout << "JMP\n";
//     {
//         as.rewind();
//         as.XOR(rax, rax);
//         as.JMP(rdi);
//         as.ADD(rax, Imm32(123));
//         auto addr = as.codeEnd();
//         as.ADD(rax, Imm32(456));
//         as.RET();
//         R64FX_EXPECT_EQ(456, fun(addr));
//     }
// 
//     cout << "JNZ\n";
//     {
//         JumpLabel loop, skip;
// 
//         as.rewind();
//         as.MOV(rax, Imm32(0));
//         as.MOV(rcx, Imm32(1234));
//         as.mark(loop);
//         as.ADD(rax, Imm32(2));
// 
//         as.JMP(skip);
//         as.ADD(rax, Imm32(1));
//         as.mark(skip);
// 
//         as.SUB(rcx, Imm32(1));
//         as.JNZ(loop);
//         as.RET();
//         R64FX_EXPECT_EQ(2468, fun(nullptr));
//     }
// 
//     cout << "\n";
//     return true;
// }
// 
// 
// bool test_sibd(Assembler &as)
// {
//     auto jitfun = (long(*)()) as.begin();
// 
//     cout << "[base + index * 8]\n";
//     {
//         as.rewindData();
//         as.growData(16 * sizeof(long));
//         auto buff = (long*) as.dataBegin();
//         buff[10] = rand() & 0xFFFF;
// 
//         as.rewind();
// 
//         as.MOV(rax, Imm32(0));
//         as.MOV(rcx, ImmAddr(buff));
//         as.MOV(rdx, Imm32(10));
//         as.MOV(rax, Base(rcx) + Index(rdx) * 8);
// 
//         as.RET();
//         R64FX_EXPECT_EQ(buff[10], jitfun());
//     }
// 
//     cout << "[base + index * 8 + Disp8]\n";
//     {
//         as.rewindData();
//         as.growData(16 * sizeof(long));
//         auto buff = (long*) as.dataBegin();
//         buff[11] = rand() & 0xFFFF;
// 
//         as.rewind();
// 
//         as.MOV(rax, Imm32(0));
//         as.MOV(rcx, ImmAddr(buff));
//         as.MOV(rdx, Imm32(10));
//         as.MOV(rax, Base(rcx) + Index(rdx) * 8 + Disp(8));
// 
//         as.RET();
//         R64FX_EXPECT_EQ(buff[11], jitfun());
//     }
// 
//     cout << "[base + index * 8 + Disp32]\n";
//     {
//         as.rewindData();
//         as.growData((128) * sizeof(long));
//         auto buff = (long*) as.dataBegin();
//         buff[10 + 64] = rand() & 0xFFFF;
// 
//         as.rewind();
// 
//         as.MOV(rax, Imm32(0));
//         as.MOV(rcx, ImmAddr(buff));
//         as.MOV(rdx, Imm32(10));
//         as.MOV(rax, Base(rcx) + Index(rdx) * 8 + Disp(64 * 8));
// 
//         as.RET();
//         R64FX_EXPECT_EQ(buff[10 + 64], jitfun());
//     }
// 
//     cout << "[base + index * 4]\n";
//     {
//         as.rewindData();
//         as.growData(32 * sizeof(float));
//         auto buff = (float*) as.dataBegin();
//         for(int i=0; i<32; i++)
//             buff[i] = float(i);
// 
//         as.rewind();
// 
//         as.MOV(rcx, ImmAddr(buff));
//         as.MOV(rdx, Imm32(4));
//         as.MOVAPS(xmm0, Base(rcx) + Index(rdx) * 4);
//         as.MOVAPS(xmm8, Base(rcx) + Index(rdx) * 8);
//         as.MOVAPS(Base(rcx), xmm0);
// 
//         as.RET();
//         jitfun();
// 
//         if(!vec_eq(buff, buff + 4, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "[index + disp8]\n";
//     {
//         as.rewindData();
//         as.growData(sizeof(long) * 8);
//         auto buff = (long*) as.dataBegin();
//         for(int i=0; i<8; i++)
//             buff[i] = long(i);
// 
//         as.rewind();
// 
//         as.MOV(rcx, ImmAddr(buff));
//         as.SAR(rcx, Imm8(1));
//         as.MOV(rax, Index(rcx)*2 + Disp(8));
//         as.SAR(rcx, Imm8(1));
//         as.MOV(Index(rcx)*4 + Disp(16), rax);
// 
//         as.RET();
//         jitfun();
// 
//         R64FX_EXPECT_EQ(buff[1], buff[2]);
//     }
// 
//     {
//         as.rewindData();
//         as.growData(sizeof(int) * 8);
//         auto buff = (int*) as.dataBegin();
//         for(int i=0; i<8; i++)
//             buff[i] = rand() & 0xFFFFFFFF;
// 
//         cout << "MOV(GPR32, Base)\n";
//         as.rewind();
//         as.MOV(rax, ImmAddr(buff + 3));
//         as.MOV(eax, Base(rax));
//         as.RET();
//         R64FX_EXPECT_EQ(buff[3], jitfun());
// 
//         cout << "MOV(GPR32, Base) + rex 1\n";
//         as.rewind();
//         as.MOV(r9, ImmAddr(buff + 3));
//         as.MOV(eax, Base(r9));
//         as.RET();
//         R64FX_EXPECT_EQ(buff[3], jitfun());
// 
//         cout << "MOV(GPR32, Base) + rex 2\n";
//         as.rewind();
//         as.MOV(r9, ImmAddr(buff + 3));
//         as.MOV(r9d, Base(r9));
//         as.MOV(rax, r9);
//         as.RET();
//         R64FX_EXPECT_EQ(buff[3], jitfun());
// 
//         cout << "MOV(Base, GPR32)\n";
//         as.rewind();
//         as.MOV(rax, Imm32(1234567));
//         as.MOV(rdx, ImmAddr(buff + 3));
//         as.MOV(Base(rdx), rax);
//         as.RET();
//         jitfun();
//         R64FX_EXPECT_EQ(1234567, buff[3]);
// 
//         cout << "MOV(Base, GPR32) + rex\n";
//         as.rewind();
//         as.MOV(rax, Imm32(555666));
//         as.MOV(r8, Imm32(121213));
//         as.MOV(rdx, ImmAddr(buff + 3));
//         as.MOV(Base(rdx), r8);
//         as.RET();
//         jitfun();
//         R64FX_EXPECT_EQ(121213, buff[3]);
//     }
// 
//     cout << "\n";
//     return true;
// }
// 
// 
// bool test_movdq(Assembler &as)
// {
//     auto jitfun = (long(*)()) as.begin();
//     as.rewindData();
//     as.growData(sizeof(float) * 16);
//     auto buff = (float*) as.dataBegin();
// 
//     cout << "MOV(D|Q)\n";
//     {
//         for(int i=0; i<8; i++)
//             buff[i] = float(rand() & 0xFFFF) * 0.01;
//         for(int i=8; i<16   ; i++)
//             buff[i] = 0.0f;
// 
//         as.rewind();
//         as.MOVAPS(xmm0, Mem128(buff));
//         as.MOVAPS(xmm8, Mem128(buff + 4));
//         as.MOVD(eax, xmm0);
//         as.MOVQ(rcx, xmm8);
//         as.MOVD(xmm8, eax);
//         as.MOVQ(xmm0, rcx);
//         as.MOVAPS(Mem128(buff + 8), xmm8);
//         as.MOVAPS(Mem128(buff + 12), xmm0);
//         as.RET();
//         jitfun();
// 
//         buff[1] = buff[2] = buff[3] = buff[6] = buff[7] = 0.0f;
// 
//         if(!vec_eq(buff, buff + 8, 4) || !vec_eq(buff + 4, buff + 12, 4))
//         {
//             return false;
//         }
//     }
// 
//     cout << "\n";
//     return true;
// }



// #define R64FX_TEST_PCMP(instr, op, type, buff)\
//     test_PCMP<type, 16/sizeof(type)>(#instr, as, (type*)buff, (&Assembler::instr), [](auto a, auto b){ return a op b; })
// 
// 
// template<typename T, int VectorSize> bool test_PCMP(
//     const char* name, Assembler &as, T* buff,
//     void (Assembler::*instr)(Xmm, Mem128),
//     bool (*expected)(T, T)
// )
// {
//     cout << name << "\n";
// 
//     auto a = buff;
//     auto b = buff + VectorSize;
//     auto r = buff + VectorSize*2;
//     auto e = buff + VectorSize*3;
// 
//     for(int i=0; i<VectorSize*2; i++)
//         buff[i] = rand() & T(-1);
//     {
//         int i = rand() & (VectorSize - 1);
//         b[i] = a[i];
//     }
//     for(int i=0; i<VectorSize; i++)
//         e[i] = (expected(a[i], b[i]) ? T(-1) : 0);
// 
//     as.rewind();
//     auto fun = (void(*)()) as.begin();
// 
//     as.MOVAPS(xmm0, Mem128(a));
//     (as.*instr)(xmm0, Mem128(b));
//     as.MOVAPS(Mem128(r), xmm0);
//     as.RET();
//     fun();
// 
//     R64FX_EXPECT_VEC_EQ(e, r, VectorSize);
//     return true;
// }
// 
// 
// bool test_pcmp(Assembler &as)
// {
//     as.rewindData();
// 
//     as.growData(64);
//     auto data = (void*)as.dataBegin();
// 
//     bool result =
//         R64FX_TEST_PCMP(PCMPEQD, ==, int,   data) &&
//         R64FX_TEST_PCMP(PCMPEQW, ==, short, data) &&
//         R64FX_TEST_PCMP(PCMPEQB, ==, char,  data) &&
//         R64FX_TEST_PCMP(PCMPGTD, >,  int,   data) &&
//         R64FX_TEST_PCMP(PCMPGTW, >,  short, data) &&
//         R64FX_TEST_PCMP(PCMPGTB, >,  char,  data);
//     cout << "\n";
//     return result;
// }
// 
// 
// #define R64FX_TEST_PUNPCK(instr, type, high)\
//     test_PUNPCK<type>(as, #instr, (&Assembler::instr), high)
// 
// template<typename T> bool test_PUNPCK(
//     Assembler &as, const char* name, void (Assembler::*instr)(Xmm, Xmm), int high
// )
// {
//     constexpr int VectorSize = 16 / sizeof(T);
// 
//     as.rewindData();
//     as.growData(32);
// 
//     cout << name << "\n";
//     {
//         auto buff = (T*)as.dataBegin();
//         auto a = buff;
//         auto b = buff + VectorSize;
//         T c[VectorSize] = {};
//         for(int i=0; i<VectorSize; i++)
//         {
//             auto val = T(rand() & T(-1));
//             c[i] = val;
//             if(i & 1)
//                 b[(i >> 1) + (high ? VectorSize/2 : 0)] = val;
//             else
//                 a[(i >> 1) + (high ? VectorSize/2 : 0)] = val;
//         }
// 
//         as.rewind();
//         auto fun = (void(*)()) as.begin();
// 
//         as.MOVDQA  (xmm0, Mem128(a));
//         as.MOVDQA  (xmm1, Mem128(b));
//         (as.*instr)(xmm0, xmm1);
//         as.MOVDQA  (Mem128(a), xmm0);
//         as.RET();
// 
//         fun();
//         R64FX_EXPECT_VEC_EQ(c, a, 4);
//     }
// 
//     return true;
// }
// 
// 
// bool test_punpck(Assembler &as)
// {
//      bool result =
//         R64FX_TEST_PUNPCK(PUNPCKLBW, char,  0) &&
//         R64FX_TEST_PUNPCK(PUNPCKHBW, char,  1) &&
//         R64FX_TEST_PUNPCK(PUNPCKLWD, short, 0) &&
//         R64FX_TEST_PUNPCK(PUNPCKHWD, short, 1) &&
//         R64FX_TEST_PUNPCK(PUNPCKLDQ, int,   0) &&
//         R64FX_TEST_PUNPCK(PUNPCKHDQ, int,   1)
//     ;
//     cout << "\n";
//     return result;
// }



// template<typename T> bool test_SSE2_SHIFT(Assembler &as, const char* name, void (Assembler::*instr)(Xmm, Imm8), T(*expected)(T, int))
// {
//     std::cout << name << "\n";
// 
//     constexpr int VectorSize = 16 / sizeof(T);
// 
//     as.rewind();
//     as.rewindData();
//     as.growData(32);
// 
//     auto buff = (T*)as.dataBegin();
//     auto a = buff;
//     auto b = buff + VectorSize;
// 
//     unsigned char s = (rand() & 0x7) + 1;
//     for(int i=0; i<VectorSize; i++)
//     {
//         /* Make sure to check for Logical/Arithmetic shifts by setting MSB. */
//         T n = T(rand() & T(-1)) | (T(1) << ((sizeof(T)<<3) - 1));
// 
//         a[i] = n;
//         b[i] = expected(n, s);
//     }
// 
//     auto fun = (void(*)())as.begin();
//     as.MOVDQA(xmm0, Mem128(a));
//     as.MOVDQA(xmm8, xmm0);
//     (as.*instr)(xmm0, Imm8(s));
//     (as.*instr)(xmm8, Imm8(s+1));
//     as.MOVDQA(Mem128(a), xmm0);
//     as.RET();
// 
//     fun();
//     R64FX_EXPECT_VEC_EQ(b, a, VectorSize);
//     return true;
// }
// 
// 
// bool test_sse2_shift(Assembler &as)
// {
// #define R64FX_TEST_SSE2_SHIFT(instr, type, op) test_SSE2_SHIFT<type>(as, #instr, (&Assembler::instr), [](type n, int s)->type{ return n op s; })
//     bool result =
//         R64FX_TEST_SSE2_SHIFT(PSRLW, unsigned short,  >>) &&
//         R64FX_TEST_SSE2_SHIFT(PSRLD, unsigned int,    >>) &&
//         R64FX_TEST_SSE2_SHIFT(PSRLQ, unsigned long,   >>) &&
//         R64FX_TEST_SSE2_SHIFT(PSRAW,          short,  >>) &&
//         R64FX_TEST_SSE2_SHIFT(PSRAD,          int,    >>) &&
//         R64FX_TEST_SSE2_SHIFT(PSLLW, unsigned short,  <<) &&
//         R64FX_TEST_SSE2_SHIFT(PSLLW,          short,  <<) &&
//         R64FX_TEST_SSE2_SHIFT(PSLLD, unsigned int,    <<) &&
//         R64FX_TEST_SSE2_SHIFT(PSLLD,          int,    <<) &&
//         R64FX_TEST_SSE2_SHIFT(PSLLQ, unsigned long,   <<) &&
//         R64FX_TEST_SSE2_SHIFT(PSLLQ,          long,   <<)
//     ;
//     cout << "\n";
//     return result;
// #undef R64FX_TEST_SSE2_SHIFT
// }
// 
// 
// bool test_movlhps(Assembler &as)
// {

// }
// 
// bool test_movlhlps(Assembler &as)
// {

// 
//     std::cout << "\n";
//     return true;
// }


struct AssemblerTestFixture : public Assembler{

float  *f0, *f1, *f2, *f3, *f4, *f5, *f6, *f7, *f8, *f9;
int    *i0, *i1, *i2, *i3, *i4, *i5, *i6, *i7, *i8, *i9;
long   *l0, *l1, *l2, *l3, *l4, *l5, *l6, *l7, *l8, *l9;

long (*lfun)  ();
long (*lfunp) (void*);
int  (*ifun)  ();
int  (*ifuni) (int*);

AssemblerTestFixture()
{
    srand(time(NULL));

    f0 = (float*)alloc_pages(1);
    f1 = f0 + 4;
    f2 = f0 + 8;
    f3 = f0 + 12;
    f4 = f0 + 16;
    f5 = f0 + 20;
    f6 = f0 + 24;
    f7 = f0 + 28;
    f8 = f0 + 32;
    f9 = f0 + 36;

    i0 = (int*)f0;
    i1 = i0 + 4;
    i2 = i0 + 8;
    i3 = i0 + 12;
    i4 = i0 + 16;
    i5 = i0 + 20;
    i6 = i0 + 24;
    i7 = i0 + 28;
    i8 = i0 + 32;
    i9 = i0 + 36;

    l0 = (long*)f0;

    resize(1);

    lfun   = (decltype(lfun))  begin();
    lfunp  = (decltype(lfunp)) begin();
    ifun   = (decltype(ifun))  begin();
    ifuni  = (decltype(ifuni)) begin();

    permitExecution();
}

~AssemblerTestFixture() { free(f0); }

bool test_mov()
{
    std::cout << "MOV(GPR64, Imm32)\n";
    {
        rewind();
        MOV(rax, Imm32(123L));
        MOV(r8,  Imm32(456L));
        RET();
        R64FX_EXPECT_EQ(123L, lfun());
    }

    std::cout << "MOV(GPR64, Imm64)\n";
    {
        rewind();
        MOV(rax, Imm64(123L));
        MOV(r8,  Imm64(456L));
        RET();
        R64FX_EXPECT_EQ(123L, lfun());
    }

    std::cout << "MOV(GPR32, Imm32)\n";
    {
        rewind();
        MOV(eax, Imm32(123L));
        MOV(r8d, Imm32(456L));
        RET();
        R64FX_EXPECT_EQ(123L, lfun());
    }

    std::cout << "MOV RIP\n";
    {
        rewind();
        i0[0] = 1212;
        i0[1] = 4567;
        i0[2] = 0;
        i0[3] = 0;
        MOV(eax, Mem32(i0));
        MOV(r8d, Mem32(i0 + 1));
        MOV(Mem32(i0 + 2), eax);
        MOV(Mem32(i0 + 3), r8d);
        RET();
        R64FX_EXPECT_EQ(1212, ifun());
        R64FX_EXPECT_EQ(1212, i0[2]);
        R64FX_EXPECT_EQ(4567, i0[3]);
    }

    std::cout << "MOV SIBD\n";
    {
        rewind();
        i0[0] = 1111;
        i0[1] = 2222;
        i0[2] = 3333;
        i0[3] = 4444;
        MOV(eax, Base(rdi));
        MOV(r8d, Base(rdi) + Disp(4));
        MOV(rcx, Imm32(1));
        MOV(Base(rdi) + Index(rcx)*4 + Disp(4), eax);
        MOV(Base(rdi) + Index(rcx)*8 + Disp(4), r8d);
        RET();
        R64FX_EXPECT_EQ(1111, ifuni(i0));
        R64FX_EXPECT_EQ(1111, i0[2]);
        R64FX_EXPECT_EQ(2222, i0[3]);
    }

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
> bool test_gpr_inst(const char* name, int (*expected)(int a, int b))
{
    cout << name << "(GPR64, Imm32)\n";
    {
        int a = rand() & 0xFFFFFF;
        int b = rand() & 0xFFFFFF;
        int c = rand() & 0xFFFFFF;

        rewind();
        MOV(rax, Imm32(a));
        (this->*gpr64_imm32)(rax, Imm32(b));
        (this->*gpr64_imm32)(r8,  Imm32(c));
        RET();
        R64FX_EXPECT_EQ(expected(a, b), ifun());
    }

    cout << name << "(GPR64, Imm8)\n";
    {
        char a = rand() & 0xFF;
        char b = rand() & 0xFF;
        char c = rand() & 0xFF;

        rewind();
        MOV(rax, Imm32(a));
        (this->*gpr64_imm8)(rax, Imm8(b));
        (this->*gpr64_imm8)(r8,  Imm8(c));
        RET();
        R64FX_EXPECT_EQ(expected(a, b), lfun());
    }

    cout << name << "(GPR64, GPR64)\n";
    {
        int a = rand() & 0xFFFFFF;
        int b = rand() & 0xFFFFFF;
        int c = rand() & 0xFFFFFF;

        rewind();
        MOV(rax, Imm32(a));
        MOV(rcx, Imm32(b));
        MOV(rdx, Imm32(c));
        (this->*gpr64_gpr64)(rax, rcx);
        (this->*gpr64_gpr64)(r8,  rdx);
        RET();
        R64FX_EXPECT_EQ(expected(a, b), lfun());
    }

    cout << name << "(GPR64, Mem64)\n";
    {
        for(int i=0; i<4; i++)
        {
            l0[i] = rand() & 0xFFFFFF;
        }

        rewind();
        MOV(rax, Imm32(0));
        MOV(rax, Mem64(i0 + 0));
        (this->*gpr64_mem64)(rax, Mem64(l0 + 1));
        (this->*gpr64_mem64)(r8,  Mem64(l0 + 2));
        RET();
        R64FX_EXPECT_EQ(expected(l0[0], l0[1]), lfun());
    }

    cout << name << "(GPR64, SIBD)\n";
    {
        for(int i=0; i<4; i++)
        {
            l0[i] = rand() & 0xFFFFFF;
        }

        rewind();
        MOV(rax, Imm32(0));
        MOV(r8,  Imm32(1));
        MOV(rax, Mem64(l0 + 0));
        MOV(rdx, ImmAddr(l0 + 1));
        MOV(rcx, Imm32(1));
        (this->*gpr64_sibd)(rax, Base(rdx) + Index(rcx) * 8 + Disp(8));
        (this->*gpr64_sibd)(r8, Base(rdx) + Index(rcx) * 8 + Disp(8));
        RET();
        R64FX_EXPECT_EQ(expected(l0[0], l0[3]), lfun());
    }

    return true;
}

template<
    typename IntT,
    void (Assembler::*shift1)     (GPR64 gpr),
    void (Assembler::*shift_imm8) (GPR64 gpr, Imm8)
>
bool test_shift_instr(const char* name, IntT (*expected)(IntT num, IntT shift))
{
    cout << name << "(GPR, 1)\n";
    {
        IntT num = rand() & 0xFFFF;
        rewind();
        MOV(rax, Imm32(num));
        (this->*shift1)(rax);
        (this->*shift1)(r8);
        RET();
        R64FX_EXPECT_EQ(expected(num, 1), lfun());
    }

    cout << name << "(GPR, Imm8)\n";
    {
        IntT num = rand() & 0xFF;
        IntT shift = ((IntT)(rand() & 0x3)) + 1;
        rewind();
        MOV(rax, Imm32(num));
        (this->*shift_imm8)(rax, Imm8(shift));
        (this->*shift_imm8)(r8,  Imm8(shift));
        RET();
        R64FX_EXPECT_EQ(expected(num, shift), lfun());
    }

    return true;
}

bool test_push_pop()
{

    cout << "PUSH & POP\n";
    {
        long num = rand();
        rewind();
        MOV(r9, Imm64(num));
        PUSH(r9);
        POP(rax);
        RET();
        R64FX_EXPECT_EQ(num, lfun());
    }

    return true;
}

bool test_jumps()
{
    cout << "JMP\n";
    {
        rewind();
        XOR(rax, rax);
        JMP(rdi);
        ADD(rax, Imm32(123));
        auto addr = ptr();
        ADD(rax, Imm32(456));
        RET();
        R64FX_EXPECT_EQ(456, lfunp(addr));
    }

    cout << "JNZ\n";
    {
        JumpLabel loop, skip;

        rewind();
        MOV(rax, Imm32(0));
        MOV(rcx, Imm32(1234));
        mark(loop);
        ADD(rax, Imm32(2));

        JMP(skip);
        ADD(rax, Imm32(1));
        mark(skip);

        SUB(rcx, Imm32(1));
        JNZ(loop);
        RET();
        R64FX_EXPECT_EQ(2468, lfunp(nullptr));
    }

    return true;
}

bool test_sse_movs()
{
    cout << "MOVAPS\n";
    {
        for(int i=0; i<4; i++)
        {
            f0[i] = float(rand());
            f1[i] = float(rand());
            f2[i] = float(rand());
            f3[i] = float(rand());
        }

        rewind();
        MOVAPS(xmm0, Mem128(f0));
        MOVAPS(xmm8, Mem128(f1));
        MOVAPS(xmm1, xmm8);
        MOVAPS(xmm9, xmm0);
        MOVAPS(Mem128(f2), xmm1);
        MOVAPS(Mem128(f3), xmm9);
        RET();

        lfun();

        R64FX_EXPECT_VEC_EQ(f2, f1, 4);
        R64FX_EXPECT_VEC_EQ(f0, f3, 4);
    }

    cout << "MOVUPS\n";
    {
        for(int i=0; i<4; i++)
        {
            f0[i+1] = float(rand());
            f1[i+1] = float(rand());
            f2[i+1] = float(rand());
            f3[i+1] = float(rand());
        }

        rewind();
        MOVUPS(xmm0, Mem32(f0+1));
        MOVUPS(xmm8, Mem32(f1+1));
        MOVUPS(xmm1, xmm8);
        MOVUPS(xmm9, xmm0);
        MOVUPS(Mem32(f2+1), xmm1);
        MOVUPS(Mem32(f3+1), xmm9);
        RET();

        lfun();

        R64FX_EXPECT_VEC_EQ(f2+1, f1+1, 4);
        R64FX_EXPECT_VEC_EQ(f0+1, f3+1, 4);
    }

    cout << "MOVLPS + MOVHPS\n";
    {
        for(int i=0; i<4; i++)
        {
            f0[i] = float(rand() & 0xFFFF);
            f1[i] = 0.0f;
        }

        rewind();
        MOVLPS(xmm0, Mem64(f0));
        MOVHPS(xmm0, Mem64(f0 + 2));
        MOVAPS(Mem128(f1), xmm0);
        RET();

        lfun();

        R64FX_EXPECT_VEC_EQ(f0, f1, 4);
    }

    std::cout << "MOVLHPS + MOVHLPS\n";
    {
        f0[0] = f0[6] = float(rand() & 0xFFF);
        f0[1] = f0[7] = float(rand() & 0xFFF);
        f0[2] = f0[4] = float(rand() & 0xFFF);
        f0[3] = f0[5] = float(rand() & 0xFFF);

        rewind();
        MOVAPS(xmm0, Mem128(f0));
        MOVLHPS(xmm1, xmm0);
        MOVHLPS(xmm1, xmm0);
        MOVAPS(Mem128(f0), xmm1);
        RET();

        lfun();

        R64FX_EXPECT_VEC_EQ(f0 + 4, f0, 4);
    }

    return true;
}

bool test()
{
    return
        test_mov() &&

#define R64FX_TEST_GPR_INSTR(name, op) test_gpr_inst<R64FX_LIST(&Assembler::name, 7)>(#name, [](int a, int b){ return a op b; })
        R64FX_TEST_GPR_INSTR(ADD, +) &&
        R64FX_TEST_GPR_INSTR(SUB, -) &&
        R64FX_TEST_GPR_INSTR(XOR, ^) &&
        R64FX_TEST_GPR_INSTR(AND, &) &&
        R64FX_TEST_GPR_INSTR(OR,  |) &&
#undef  R64FX_TEST_GPR_INSTR

#define R64FX_TEST_SHIFT_INSTR(type, name, op) test_shift_instr<type, R64FX_LIST(&Assembler::name, 2)>(#name, [](type num, type shift){ return num op shift; })
        R64FX_TEST_SHIFT_INSTR(int,          SHL, <<) &&
        R64FX_TEST_SHIFT_INSTR(int,          SHR, >>) &&
        R64FX_TEST_SHIFT_INSTR(unsigned int, SAR, >>) &&
#undef  R64FX_TEST_SHIFT_INSTR

        test_push_pop() &&

        test_jumps() &&

        test_sse_movs() &&

        true;
}};


#define R64FX_TEST(INSTR) () &&

int main()
{
    if(AssemblerTestFixture().test())
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
