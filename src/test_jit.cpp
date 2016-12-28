#include "jit.hpp"
#include <iostream>

using namespace std;
using namespace r64fx;

typedef long (*JitFun)();

void* g_data = nullptr;

bool test1(Assembler &as)
{
    long num = 123456789;

    as.rewindIp();
    as.mov(rax, Imm64(num));
    as.ret();

    auto jitfun = (JitFun) as.codeBegin();
    auto res = jitfun();
    if(res != num)
    {
        cout << "test1: Failed!\n";
        cout << "   Expected: " << num << ", Got: " << res << "\n";
        return false;
    }
    return true;
}


bool test2(Assembler &as)
{
    auto a = (float*) g_data;
    auto b = a + 4;
    a[0] = 1.0f;
    a[1] = 2.0f;
    a[2] = 3.0f;
    a[3] = 4.0f;

    b[0] = 0.1f;
    b[1] = 0.2f;
    b[2] = 0.3f;
    b[3] = 0.4f;

    float c[4] = {1.1f, 2.2f, 3.3f, 4.4f};

    as.rewindIp();
    as.movaps (xmm0, Mem128(a));
    as.movaps (xmm1, Mem128(b));
    as.addps  (xmm0, xmm1);
    as.movaps (Mem128(a), xmm0);
    as.ret();

    auto jitfun = (JitFun) as.codeBegin();
    jitfun();

    for(int i=0; i<4; i++)
    {
        if(a[i] != c[i])
        {
            cout << "test2: Failed!\n";
            cout << "   i: " << i << " -> Expected: " << c[i] << ", Got: " << a[i] << "\n";
            return false;
        }
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

    auto ok =
        test1(as) &&
        test2(as)
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
