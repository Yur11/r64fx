#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


float v1[8] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
float v2[4] = { 0.1, 0.1, 0.1, 0.1 };

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

    a.mov(rax, Imm64(&v1));
    a.mov(rbx, Imm64(&v2));

    a.movaps(xmm0, Base(rax));
    a.movaps(xmm1, Base(rbx));

    a.addps(xmm0, xmm1);

    a.movaps(Base(rax), xmm0);
    a.movaps(Base(rbx), xmm1);

    a.ret();

    typedef float (*Fun)();
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