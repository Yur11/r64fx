#include <iostream>
#include "jit.hpp"

using namespace std;
using namespace r64fx;

template<typename T> inline void fill(T* buff, T val)
    { for(int i=0; i<4; i++) buff[i] = val; }

template<typename T> void dump(T data)
{
    for(int i=0; i<4; i++)
        cout << data[i] << (i < 3 ? ", " : "\n");
}

int main()
{
    auto f0 = (float*) alloc_pages(1);
    auto f1 = f0 + 4;
    auto f2 = f0 + 8;
    auto f3 = f0 + 12;
    auto f4 = f0 + 16;

    fill(f0, 0.0f);
    fill(f1, 1.0f);
    fill(f2, 2.0f);
    fill(f3, 3.0f);
    fill(f4, 4.0f);

    Assembler as; as.resize(1);
    as.MOVAPS(xmm0, Mem128(f3));
    as.MOVAPS(Mem128(f1), xmm0);
    as.MOV(rax, Imm32(0));
    as.RET();

    auto fun = (long(*)(float*, float*)) as.begin();

    as.permitExecution();
    cout << fun(f0, f2) << "\n";

    dump(f0);
    dump(f1);
    dump(f2);
    dump(f3);
    dump(f4);

    free(f0);

    cout << "OK\n";
    return 0;
}
