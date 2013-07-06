#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


float v1[4] = { 1.0, 2.0, 3.0, 4.0 };
float v2[4] = { 0.1, 0.1, 0.1, 0.1 };

ostream &operator<<(ostream &ost, float vec[4])
{
    ost << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3];
    return ost;
}

int main()
{
    Assembler a;
    a.push(1000);
    a.mov(rax, 4);
    a.pop(rax);
    a.ret();

    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    cout << "-----\n";
    cout << "fun: [" << fun() << "]\n";
    cout << "-----\n";

    return 0;
}