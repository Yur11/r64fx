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

    cout << "mov(gpr64, imm32)\n";
    {
        int num = rand();
        as.rewindIp();
        as.mov(rax, Imm32(num));
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(gpr64, imm32) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindIp();
        as.mov(rax, Imm32(num1));
        as.mov(r8,  Imm32(num2));
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun())
    }

    cout << "mov(gpr64, imm64)\n";
    {
        long int num = rand();
        as.rewindIp();
        as.mov(rax, Imm64(num));
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(gpr64, imm64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        as.rewindIp();
        as.mov(rax, Imm64(num1));
        as.mov(r8,  Imm64(num2));
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "mov(gpr64, gpr64)\n";
    {
        int num = rand();
        as.rewindIp();
        as.mov(rcx, Imm32(num));
        as.mov(rax, rcx);
        as.ret();
        R64FX_EXPECT_EQ(num, jitfun());
    }

    cout << "mov(gpr64, gpr64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        int num3 = rand();
        as.rewindIp();
        as.mov(r9,  Imm32(num1));
        as.mov(rdx, Imm32(num2));
        as.mov(rcx, Imm32(num3));
        as.mov(rax, r9);
        as.mov(r8,  rdx);
        as.ret();
        R64FX_EXPECT_EQ(num1, jitfun());
    }

    cout << "mov(gpr64, mem64)\n";
    {
        *a = rand();
        as.rewindIp();
        as.mov(rax, Mem64(a));
        as.ret();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "mov(gpr64, mem64) rex\n";
    {
        *a = rand();
        *b = rand();
        as.rewindIp();
        as.mov(rax, Mem64(a));
        as.mov(r8,  Mem64(b));
        as.ret();
        R64FX_EXPECT_EQ(*a, jitfun());
    }

    cout << "mov(mem64, gpr64)\n";
    {
        int num = rand();
        *a = rand();
        as.rewindIp();
        as.mov(rcx, Imm32(num));
        as.mov(Mem64(a), rcx);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(num, *a);
    }

    cout << "mov(mem64, gpr64) rex\n";
    {
        int num1 = rand();
        int num2 = rand();
        *a = 0;
        as.rewindIp();
        as.mov(rax, Imm32(num1));
        as.mov(r8,  Imm32(num2));
        as.mov(Mem64(a), r8);
        as.ret();
        jitfun();
        R64FX_EXPECT_EQ(num2, *a);
    }

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

    auto ok = true;
    ok = ok && test_mov(as);

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
