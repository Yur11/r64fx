#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


float v1[4] = { 1.0, 2.0, 3.0, 4.0 };
float v2[4] = { 0.1, 0.1, 0.1, 0.1 };

long int iv3[4] = { 11, 22, 33, 44 };


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
    a.push(rbp);

    a.movups(xmm0, Mem128(&v1));
    a.mov(rbp, Imm64(&v2));
    a.addps(xmm0, Base(rbp));

    a.pop(rbp);
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