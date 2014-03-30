#include <iostream>
#include <iomanip>
#include "jit.h"

using namespace std;
using namespace r64fx;

typedef long int (*Fun)(void);

int a[4] = { 0, 0, 0, 0 };
int b[8] = { 1, 2, 3, 4, -4, -3, 1, -1 };


template<typename T>
void dump(T* p)
{
    for(int i=0; i<3; i++)
        cout << p[i] << ", ";
    cout << p[3] << "\n";
}

int main()
{
    CodeBuffer cb;
    Assembler as(cb);

    as.movaps(xmm0, Mem128(a));
    as.mov(rbx, Imm64(b));
    as.mov(rcx, Imm64(101010101));
    auto loop = Mem8(as.ip());
        as.psubd(xmm0, Base(rbx), Disp8(4*sizeof(int)));
        as.sub(rcx, 1);
        as.jnz(loop);
    as.movaps(Mem128(a), xmm0);
    
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