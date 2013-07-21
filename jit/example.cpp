#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


union CpuCycleCount{
    unsigned int parts[2];
    unsigned long value;
} begin_count, end_count;

int num1 = 123;
int num2 = 456;

int main()
{
    Assembler a;
    a.rdtsc();
    a.mov(Mem32((&begin_count.parts) + 0), eax);
    a.mov(Mem32((&begin_count.parts) + 1), edx);

    a.mov(r8, num1);
    a.mov

    a.rdtsc();
    a.mov(Mem32((&end_count.parts) + 0), eax);
    a.mov(Mem32((&end_count.parts) + 1), edx);
    a.ret();

    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    for(int i=0; i<100; i++)
    {
        cout << "-----\n";
        cout << "fun: [" << fun() << "]\n";
        cout << "> " << begin_count.value << "\n";
        cout << "> " << end_count.value << "\n";
        cout << "> " << end_count.value - begin_count.value << "\n";
        cout << "-----\n";
    }

    return 0;
}