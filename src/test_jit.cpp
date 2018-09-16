/* Test overall operation.
 *
 * Test REX prefix. Incorrect prefix order may cause cpu to ignore REX prefix altogether
 * thus encoding a wrong register.
 * Checking for rax being encoded instead r8, rcx instead of r9 etc...
 */

#include "test.hpp"
#include "jit.hpp"
#include <iostream>
#include <cmath>

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

template<typename T> inline int scalars_per_vec() { return 0; }
template<> inline int scalars_per_vec <char>           () { return 16; }
template<> inline int scalars_per_vec <unsigned char>  () { return 16; }
template<> inline int scalars_per_vec <short>          () { return  8; }
template<> inline int scalars_per_vec <unsigned short> () { return  8; }
template<> inline int scalars_per_vec <int>            () { return  4; }
template<> inline int scalars_per_vec <unsigned int>   () { return  4; }
template<> inline int scalars_per_vec <float>          () { return  4; }
template<> inline int scalars_per_vec <long>           () { return  2; }
template<> inline int scalars_per_vec <unsigned long>  () { return  2; }
template<> inline int scalars_per_vec <double>         () { return  2; }

template<typename T> inline T ADD  (T a, T b) { return a+b; }
template<typename T> inline T SUB  (T a, T b) { return a-b; }
template<typename T> inline T MUL  (T a, T b) { return a*b; }
template<typename T> inline T DIV  (T a, T b) { return a/b; }
template<typename T> inline T OR   (T a, T b) { return a|b; }
template<typename T> inline T AND  (T a, T b) { return a&b; }
template<typename T> inline T XOR  (T a, T b) { return a^b; }
template<typename T> inline T MAX  (T a, T b) { return a>b?a:b; }
template<typename T> inline T MIN  (T a, T b) { return a<b?a:b; }
template<typename T> inline T SQRT (T, T val) { return sqrt(val); }

int    f2i(float f) { union{ int i; float f; }u; u.f=f; return u.i; }
float  i2f(int   i) { union{ int i; float f; }u; u.i=i; return u.f; }

float FOR   (float a, float b) { return i2f(f2i(a)  | f2i(b)); }
float FAND  (float a, float b) { return i2f(f2i(a)  & f2i(b)); }
float FXOR  (float a, float b) { return i2f(f2i(a)  ^ f2i(b)); }
float FANDN (float a, float b) { return i2f(~f2i(a) & f2i(b)); }

class AssemblerTestFixture : public Assembler{

unsigned char* m_data = nullptr;

public:

#define R64FX_DATA(T) inline T* data(T) { return (T*)m_data; }
R64FX_DATA(char)
R64FX_DATA(unsigned char)
R64FX_DATA(short)
R64FX_DATA(unsigned short)
R64FX_DATA(int)
R64FX_DATA(unsigned int)
R64FX_DATA(long)
R64FX_DATA(unsigned long)
R64FX_DATA(float)
R64FX_DATA(double)
#undef R64FX_DATA

long  (*lfun)  ();
long  (*lfunp) (void*);
int   (*ifun)  ();
int   (*ifuni) (int*);
float (*ffun)  ();

AssemblerTestFixture()
{
    srand(time(NULL));

    m_data = (unsigned char*)alloc_pages(1);

    resize(1);

    lfun   = (decltype(lfun))  begin();
    lfunp  = (decltype(lfunp)) begin();
    ifun   = (decltype(ifun))  begin();
    ifuni  = (decltype(ifuni)) begin();
    ffun   = (decltype(ffun))  begin();

    permitExecution();
}

~AssemblerTestFixture() { free(m_data); }

bool test_mov()
{
    auto i0 = data(int());

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

bool test_lea()
{
    cout << "LEA RIP\n";
    rewind();
    LEA(rax, Mem8(begin()));
    RET();
    R64FX_EXPECT_EQ((long)begin(), lfun());

    cout << "LEA SIB\n";
    rewind();
    MOV(rcx, Imm32(123));
    MOV(rdx, Imm32(1));
    LEA(rax, Base(rcx) + Index(rdx)*8 + Disp(-1));
    RET();
    R64FX_EXPECT_EQ(130, lfun());

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
    auto i0 = data(int());
    auto l0 = data(long());

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
bool test_gpr_shift(const char* name, IntT (*expected)(IntT num, IntT shift))
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

template<typename IntT, void (Assembler::*shift_imm8) (Xmm, Imm8)>
bool test_sse_shift(const char* name, IntT (*expected)(IntT num, IntT shift))
{
    cout << name << "\n";

    auto p = data(IntT());
    auto s = scalars_per_vec<IntT>();

    for(int i=0; i<s; i++)
        p[i] = rand() & IntT(-1);

    for(int i=s; i<s*2; i++)
        p[i] = 0;

    IntT shift = ((IntT)(rand() & 0x3)) + 1;

    for(int i=0; i<s; i++)
        p[i + s*2] = expected(p[i], shift);

    rewind();
    MOVDQA(xmm0, Mem128(p));
    (this->*shift_imm8)(xmm0, Imm8(shift));
    MOVDQA(Mem128(p+s), xmm0);
    RET();
    lfun();

    R64FX_EXPECT_VEC_EQ(p+s*2, p+s, s);
    return true;
}

bool test_push_pop()
{
    cout << "PUSH & POP\n";

    long num = rand();
    rewind();
    MOV(r9, Imm64(num));
    PUSH(r9);
    POP(rax);
    RET();
    R64FX_EXPECT_EQ(num, lfun());

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
    auto f0 = data(float());
    auto f1 = data(float()) + 4;
    auto f2 = data(float()) + 8;
    auto f3 = data(float()) + 12;

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

bool test_sse2_movs()
{
    auto f = data(float());

    std::cout << "MOVD(Xmm, Xmm)\n";
    {
        float val = float((rand() & 0xFFFF) + 1);

        rewind();
        XORPS (xmm0, xmm0);
        MOV   (rax, Imm64f(val));
        MOVD  (xmm0, eax);
        RET   ();

        R64FX_EXPECT_EQ(val, ffun());
    }

    std::cout << "MOVD(Xmm, Mem32)\n";
    {
        float val = float((rand() & 0xFFFF) + 1);
        f[0] = val;
        f[1] = 0.0f;

        rewind();
        XORPS (xmm0, xmm0);
        MOVD  (xmm0, Mem32(f));
        MOVD  (Mem32(f+1), xmm0);
        RET   ();

        R64FX_EXPECT_EQ(val, ffun());
        R64FX_EXPECT_EQ(val, f[1]);
    }

    std::cout << "MOVD(Xmm, SIBD)\n";
    {
        float val = float((rand() & 0xFFFF) + 1);
        f[0] = val;
        f[1] = 0.0f;

        rewind();
        XORPS (xmm0, xmm0);
        MOV   (rax, ImmAddr(f));
        MOVD  (xmm0, Base(rax));
        MOVD  (Base(rax) + Disp(4), xmm0);
        RET   ();

        R64FX_EXPECT_EQ(val, ffun());
        R64FX_EXPECT_EQ(val, f[1]);
    }

    return true;
}

template<
    typename T,
    void (Assembler::*xmmxmm)(Xmm, Xmm),
    void (Assembler::*xmmrip)(Xmm, Mem128),
    void (Assembler::*xmmsib)(Xmm, SIBD)
>
bool test_sse_instr(const char* name, T (*expected)(T, T))
{
    auto p = data(T());
    auto s = scalars_per_vec<T>();

    cout << name << "\n";
    for(int i=0; i<s*6; i++)
        p[i] = T((rand() & 0xFF) + 1);

    for(int i=0; i<4; i++)
        p[i + s*5] = expected(expected(expected(p[i], p[i + s]), p[i + s*2]), p[i + s*3]);

    rewind();
    MOVAPS          (xmm0, Mem128(p));
    MOVAPS          (xmm8, Mem128(p + s));
    (this->*xmmxmm) (xmm0, xmm8);
    (this->*xmmrip) (xmm0, Mem128(p + s*2));
    (this->*xmmsib) (xmm0, Base(rdi));
    MOVAPS          (Mem128(p + s*4), xmm0);
    RET();

    lfunp(p + s*3);

    R64FX_EXPECT_VEC_EQ(p + s*5, p + s*4, s);

    return true;
}

bool test_pshufd()
{
    cout << "PSHUFD\n";

    auto p = data(int());

    for(int i=0; i<4; i++)
        p[i] = rand();

    for(int i=4; i<16; i++)
        p[i] = 0.0f;

    unsigned char shuf_byte = rand() & 0xFF;

    rewind();
    MOVAPS(xmm0, Mem128(p));
    PSHUFD(xmm0, xmm0, shuf_byte);
    MOVAPS(Mem128(p+4), xmm0);
    RET();
    ifun();

    for(int i=0; i<4; i++)
    {
        p[i + 8] = p[shuf_byte & 3];
        shuf_byte >>= 2;
    }

    R64FX_EXPECT_VEC_EQ(p+8, p+4, 4);

    return true;
}

bool test()
{
    return
        test_mov() &&
        test_lea() &&

#define R64FX_TEST_GPR_INSTR(name, op) test_gpr_inst<R64FX_LIST(&Assembler::name, 7)>(#name, [](int a, int b){ return a op b; })
        R64FX_TEST_GPR_INSTR(ADD, +) &&
        R64FX_TEST_GPR_INSTR(SUB, -) &&
        R64FX_TEST_GPR_INSTR(XOR, ^) &&
        R64FX_TEST_GPR_INSTR(AND, &) &&
        R64FX_TEST_GPR_INSTR(OR,  |) &&
#undef  R64FX_TEST_GPR_INSTR

#define R64FX_TEST_GPR_SHIFT(type, name, op) test_gpr_shift<type, R64FX_LIST(&Assembler::name, 2)>(#name, [](type num, type shift){ return num op shift; })
        R64FX_TEST_GPR_SHIFT(int,          SHL, <<) &&
        R64FX_TEST_GPR_SHIFT(int,          SHR, >>) &&
        R64FX_TEST_GPR_SHIFT(unsigned int, SAR, >>) &&
#undef  R64FX_TEST_GPR_SHIFT

        test_push_pop()  &&
        test_jumps()     &&
        test_sse_movs()  &&
        test_sse2_movs() &&

#define R64FX_TEST_SSE_INSTR(type, name, fun) test_sse_instr<type, R64FX_LIST(&Assembler::name, 3)>(#name, ::fun)
        R64FX_TEST_SSE_INSTR(float, ADDPS,  ADD   ) &&
        R64FX_TEST_SSE_INSTR(float, SUBPS,  SUB   ) &&
        R64FX_TEST_SSE_INSTR(float, MULPS,  MUL   ) &&
        R64FX_TEST_SSE_INSTR(float, DIVPS,  DIV   ) &&
        R64FX_TEST_SSE_INSTR(float, MAXPS,  MAX   ) &&
        R64FX_TEST_SSE_INSTR(float, MINPS,  MIN   ) &&
        R64FX_TEST_SSE_INSTR(float, SQRTPS, SQRT  ) &&
        R64FX_TEST_SSE_INSTR(float, ORPS,   FOR   ) &&
        R64FX_TEST_SSE_INSTR(float, ANDPS,  FAND  ) &&
        R64FX_TEST_SSE_INSTR(float, XORPS,  FXOR  ) &&
        R64FX_TEST_SSE_INSTR(float, ANDNPS, FANDN ) &&
        R64FX_TEST_SSE_INSTR(int,   PADDD,  ADD   ) &&
        R64FX_TEST_SSE_INSTR(int,   PSUBD,  SUB   ) &&
        R64FX_TEST_SSE_INSTR(long,  PADDQ,  ADD   ) &&
        R64FX_TEST_SSE_INSTR(long,  PSUBQ,  SUB   ) &&
        R64FX_TEST_SSE_INSTR(int,   PAND,   AND   ) &&
        R64FX_TEST_SSE_INSTR(int,   POR,    OR    ) &&
        R64FX_TEST_SSE_INSTR(int,   PXOR,   XOR   ) &&
#undef  R64FX_TEST_SSE_INSTR

        test_pshufd() &&

#define R64FX_TEST_SSE_SHIFT(type, name, op) test_sse_shift<type, &Assembler::name>(#name, [](type num, type shift) -> type { return num op shift; })
        R64FX_TEST_SSE_SHIFT(unsigned short, PSRLW, >>) &&
        R64FX_TEST_SSE_SHIFT(unsigned int,   PSRLD, >>) &&
        R64FX_TEST_SSE_SHIFT(unsigned long,  PSRLQ, >>) &&
        R64FX_TEST_SSE_SHIFT(short,          PSRAW, >>) &&
        R64FX_TEST_SSE_SHIFT(int,            PSRAD, >>) &&
        R64FX_TEST_SSE_SHIFT(short,          PSLLW, <<) &&
        R64FX_TEST_SSE_SHIFT(int,            PSLLD, <<) &&
        R64FX_TEST_SSE_SHIFT(long,           PSLLQ, <<) &&
#undef  R64FX_TEST_SSE_SHIFT

        true;
}};


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
