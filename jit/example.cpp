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

    a.mov(rax, 4);
    a.mov(rsi, Imm64(&iv3));
    a.add(rax, Base(rsi));
    a.add(Base(rsi), rax);

    a.pop(rbp);
    a.ret();

    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    cout << "-----\n";
    cout << iv3 << "\n";
    cout << "fun: [" << fun() << "]\n";
    cout << iv3 << "\n";
    cout << "-----\n";

    return 0;
}