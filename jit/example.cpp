#include <iostream>
#include "jit.h"

using namespace std;
using namespace r64fx;


// union CpuCycleCount{
//     unsigned int parts[2];
//     unsigned long value;
// } begin_count, end_count;

// int num1 = 123;
// int num2 = 456;
// 
// float freq[4] = {110.0, 220.0, 440.0, 880.0};
// float osc[4] = {0.0, 0.0, 0.0, 0.0};
// const float samplerate_rcp = 1.0 / 48000.0;
// float samplerate_rcp_buff[4] = {samplerate_rcp, samplerate_rcp, samplerate_rcp, samplerate_rcp};
// float one[4] = {1.0, 1.0, 1.0, 1.0};
// float two[4] = {2.0, 2.0, 2.0, 2.0};
// const int nsamples = 1024;
// float buffer[nsamples * 4];


float vec1[8] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };


int main()
{
    Assembler a;

//     a.rdtsc();
//     a.mov(Mem32((&begin_count.parts) + 0), eax);
//     a.mov(Mem32((&begin_count.parts) + 1), edx);
// 
//     a.mov(rcx, nsamples);
//     a.mov(rax, Imm64((long int)buffer));
//     
//     auto loop = Mem8(a.ip());
//         /* Calculate period length. */
//         a.movaps(xmm0, Mem128(freq));
//         
//         /* Calculate per sample value delta.*/
//         a.mulps(xmm0, Mem128(samplerate_rcp_buff));
//         
//         /* Fetch previous oscillator value. */
//         a.movaps(xmm1, Mem128(osc));
//         
//         /* Add delta to the the previous oscillator value.. */
//         a.addps(xmm1, xmm0);
//         
//         /* Subtract 1 if the new value is grater than 1. */
//         a.movaps(xmm0, Mem128(one));
//         a.cmpps(LT, xmm0, xmm1);
//         a.andps(xmm0, Mem128(one));
//         a.subps(xmm1, xmm0);
//         
//         /* Save new oscillator value. */
//         a.movaps(Mem128(osc), xmm1);
//     
//         a.movaps(xmm0, xmm1);
//         a.mulps(xmm1, xmm0);
//         a.mulps(xmm1, xmm0);
//         a.movaps(xmm0, xmm1);
//         a.movaps(xmm1, Mem128(one));
//         a.subps(xmm1, xmm0);
//         a.mulps(xmm1, Mem128(two));
//         a.subps(xmm1, Mem128(one));
//         
//         /* Write result to buffer. */
//         a.movaps(Base(rax), xmm1);
//         
//         a.add(rax, sizeof(float) * 4);
//         a.sub(rcx, 1);
//         a.jnz(loop);
//         
//     a.rdtsc();
//     a.mov(Mem32((&end_count.parts) + 0), eax);
//     a.mov(Mem32((&end_count.parts) + 1), edx);
//     
//     a.ret();
    
//     a.movaps(xmm1, Mem128(vec1));
    a.mov(rdx, Imm64(vec1));
    a.pshufd(xmm0, Base(rdx), Disp8(16), shuf(3, 2, 1, 0));
    a.movaps(Mem128(vec1), xmm0);
    a.ret();
    
    typedef long int (*Fun)();
    Fun fun = (Fun) a.getFun();

    cout << vec1[0] << ", " << vec1[1] << ", " << vec1[2] << ", " << vec1[3] << "\n";
    fun();
    cout << vec1[0] << ", " << vec1[1] << ", " << vec1[2] << ", " << vec1[3] << "\n";
    
    
    
//     for(int i=0; i<nsamples; i++)
//     {
//         for(int n=0; n<3; n++)
//         {
//             cout << buffer[i*4 + n] << ", ";
//         }
//         cout << buffer[i*4 + 3] << "\n";
//     }
    
//     cout << "> " << begin_count.value << "\n";
//     cout << "> " << end_count.value << "\n";
//     cout << "> " << end_count.value - begin_count.value << "\n";
//     

    return 0;
}