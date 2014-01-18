#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;

typedef long int (*Fun)(void);

unsigned int begin_time_low, begin_time_high, end_time_low, end_time_high;

float buffer[4] = {11.11, 11.11, 11.11, 11.11};
float val = 1234.567;

int main()
{
    CodeBuffer cb;
    Assembler as(cb);
    
    as.rdtsc();
    as.mov(Mem32(&begin_time_low), eax);
    as.mov(Mem32(&begin_time_high), edx);
    
    as.movups(xmm0, Mem128(buffer));
    as.addps(xmm0, xmm0);
    as.movss(xmm0, Mem32(&val));
    as.movups(Mem128(buffer), xmm0);
    as.movss(Mem32(&val), xmm0);
        
    as.rdtsc();
    as.mov(Mem32(&end_time_low), eax);
    as.mov(Mem32(&end_time_high), edx);
    
    as.ret();
    
    cout << as.dump.str() << "\n";
    cout << "---------------------\n\n";
    
    Fun fun = (Fun) as.getFun();

    cout << "fun: " << fun() << "\n";

    for(int i=0; i<4; i++)
    {
        cout << buffer[i] << "\n";
    }
    
    cout << "val: " << val << "\n";
    
    auto begin_time = begin_time_low + (begin_time_high << 31);
    auto end_time = end_time_low + (end_time_high << 31);
    
    cout << "\n";
    cout << begin_time << "\n";
    cout << end_time << "\n";
    cout << end_time - begin_time << "\n";
    
    return 0;
}