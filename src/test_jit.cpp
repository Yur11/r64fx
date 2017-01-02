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

    return true;
}


bool test_add(Assembler &as)
{
    auto jitfun = (JitFun) as.codeBegin();

    auto buff = (long int*) g_data;
    auto a = buff;
    auto b = buff + 1;

    srand(time(NULL));

    cout << "add(GPR64, Imm32) + rex\n";
    {
        int num1 = rand() % 10000;
        int num2 = rand() % 10000;
        int num3 = rand() % 10000;
        int sum = num1 + num2;
        as.rewindIp();
        as.mov(rax, Imm32S(num1));
        as.add(rax, Imm32S(num2));
        as.mov(r8,  Imm32S(0));
        as.add(r8,  Imm32S(num3));
        as.ret();
        R64FX_EXPECT_EQ(sum, jitfun());
    }

//     cout << "add(GPR64, GPR64) + rex\n";
//     {
//         int num1 = rand() % 10000;
//         int num2 = rand() % 10000;
//         int num3 = rand() % 10000;
//         int num3 = rand() % 10000;
//         int sum = num1 + num2;
//         as.rewindIp();
//         as.mov(rax, Imm32(num1));
//         as.mov(rcx, Imm32(num2));
//         as.mov(rdx, Imm32(num3));
//         as.add(rax);
//         as.ret();
//     }

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
    ok = ok && test_add(as);

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
