#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


ostream &operator<<(ostream &ost, float vec[4])
{
    ost << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3];
    return ost;
}


ostream &operator<<(ostream &ost, long int vec[4])
{
    ost << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3];
    return ost;
}


int main()
{
    long int* buff = new long int[32];

    Assembler a;
    a.mov(rax, 0);
    a.mov(rbx, ImmPtr(buff));
    auto loop = Mem8(a.ip());
        a.add(rax, 1);
        a.mov(Base(rbx), rax);
        a.add(rbx, sizeof(long int));
        a.cmp(rax, Imm32(32));
        a.jl(loop);
    a.ret();

    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    cout << "-----\n";
    cout << "fun: [" << fun() << "]\n";
    cout << "-----\n";

    for(int i=0; i<32; i++)
    {
        cout << buff[i] << "\n";
    }

    delete[] buff;

    return 0;
}