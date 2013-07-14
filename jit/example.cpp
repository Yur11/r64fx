#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


float v1[8] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
float v2[4] = { 1.0, 1.1, 1.2, 2.0 };

long int iv3[4] = { 11, 22, 33, 44 };

long int i1 = 123;
long int i2 = 456;

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
    Assembler a;
    a.mov(rax, 0);
    a.mov(r9,  10);
    auto loop = Mem8(a.ip());
        a.add(rax, 1);
        a.sub(r9, 2);
        a.jz(Mem8(a.ip() + 12)); //Jump into one of the nops below =)
        a.jmp(loop);
    a.nop(10);
    a.ret();

    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    cout << "-----\n";
    cout << "v1:  " << v1 << "\n";
    cout << "v2:  " << v2 << "\n";
    cout << "fun: [" << fun() << "]\n";
    cout << "v1:  " << v1 << "\n";
    cout << "v2:  " << v2 << "\n";
    cout << "-----\n";

    return 0;
}