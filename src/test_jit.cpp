#include "jit.hpp"
#include <iostream>

using namespace std;
using namespace r64fx;

typedef long (*JitFun)();

void* g_data = nullptr;

bool test_add64(Assembler &as)
{
    auto buff = (long int*) g_data;
    auto a = buff;
    auto b = buff + 1;

    auto aval = 13;
    auto bval = 25;
    *a = aval;
    *b = bval;

    as.rewindIp();
    as.mov(rax, Imm64(0));
    as.mov(rcx, Imm64(0));

    as.add(rax, Mem64(a));
    as.mov(rdx, Imm64((unsigned long)a));
    as.add(rcx, Base(rdx), Disp8(8));
    as.mov(Mem64(a), rcx);
    as.add(rax, rcx);

    as.ret();

    auto jitfun = (JitFun) as.codeBegin();
    int result = jitfun();
    cout << result << ", " << *a << "\n";

    int expected_result = (aval + bval);
    if(result != expected_result)
    {
        cerr << "test_add64.1: Failed!\nExpected: " << expected_result << ", Got: " << result << "\n";
        return false;
    }

    if(*a != bval)
    {
        cerr << "test_add64.2: Failed!\nExpected: " << bval << ", Got: " << *a << "\n";
        return false;
    }

    return true;
}


// bool test2(Assembler &as)
// {
//     auto a = (float*) g_data;
//     auto b = a + 4;
//     a[0] = 1.0f;
//     a[1] = 2.0f;
//     a[2] = 3.0f;
//     a[3] = 4.0f;
// 
//     b[0] = 0.1f;
//     b[1] = 0.2f;
//     b[2] = 0.3f;
//     b[3] = 0.4f;
// 
//     float c[4] = {1.1f, 2.2f, 3.3f, 4.4f};
// 
//     as.rewindIp();
//     as.movaps (xmm0, Mem128(a));
//     as.movaps (xmm1, Mem128(b));
//     as.addps  (xmm0, xmm1);
//     as.movaps (Mem128(a), xmm0);
//     as.ret();
// 
//     auto jitfun = (JitFun) as.codeBegin();
//     jitfun();
// 
//     for(int i=0; i<4; i++)
//     {
//         if(a[i] != c[i])
//         {
//             cout << "test2: Failed!\n";
//             cout << "   i: " << i << " -> Expected: " << c[i] << ", Got: " << a[i] << "\n";
//             return false;
//         }
//     }
//     return true;
// }


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
    ok = ok && test_add64(as);

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
