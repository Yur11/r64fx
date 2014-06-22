#include <iostream>
#include <iomanip>
#include "jit.h"

using namespace std;
using namespace r64fx;

typedef long int (*Fun)(void);

int samplerate = 192000;

float sr[4] = { float(samplerate), float(samplerate)*2, float(samplerate)*4, float(samplerate)*8 };
float zero[4] = { 0, 0, 0, 0 };
float one[4] = { 1.0, 1.0, 1.0, 1.0 };

float a[4] = { 0, 0, 0, 0 };
float b[4] = { 0, 0, 0, 0 };


template<typename T>
void dump(T* p)
{
    for(int i=0; i<3; i++)
        cout << p[i] << ", ";
    cout << p[3] << "\n";
}

int main()
{
    cout << fixed << setprecision(15);
    
    CodeBuffer cb;
    Assembler as(cb);

    /* Calculate reciprocal. */
    as.movaps(xmm0, Mem128(one));
    as.divps(xmm0, Mem128(sr));
    as.movaps(Mem128(a), xmm0);

    /** Simple */
//     as.movaps(xmm1, Mem128(zero));
//     as.mov(rcx, samplerate);
//     Mem8 l = as.ip();
//         as.addps(xmm1, xmm0);
//         as.sub(rcx, 1);
//         as.jnz(l);


    /** Kahan */
    as.movaps(xmm1, zero); //sum
    as.movaps(xmm2, xmm1); //correction
    as.mov(rcx, samplerate);
    Mem8 l = as.ip();
        /* var y = input - c */
        as.movaps(xmm3, xmm0);
        as.subps(xmm3, xmm2);
    
        /* var t = sum + y */
        as.movaps(xmm4, xmm1);
        as.addps(xmm4, xmm3);
        
        /* c = (t-sum) - y */
        as.movaps(xmm2, xmm4);
        as.subps(xmm2, xmm1);
        as.subps(xmm2, xmm3);
        
        /* sum = t */
        as.movaps(xmm1, xmm4);
        
        as.sub(rcx, 1);
        as.jnz(l);

    as.movaps(b, xmm1);
        
    as.ret();
    
    Fun fun = (Fun) as.getFun();
    
    dump(a);
    dump(b);
    
    cout << "---------\n";
    
    fun();
    
    dump(a);
    dump(b);
    
    return 0;
}