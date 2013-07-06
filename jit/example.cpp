#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


int num1 = 20;
int num2 = 40;


int main()
{
    Assembler a;
    a.mov(rax, 111);
    a.mov(r10, 123);
    a.add(rax, r10);
    a.ret();

    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    cout << "-----\n";
    cout << "num1: " << num1 << "\n";
    cout << "num2: " << num2 << "\n";
    cout << "fun: [" << fun() << "]\n";
    cout << "num1: " << num1 << "\n";
    cout << "num2: " << num2 << "\n";
    cout << "-----\n";

    return 0;
}