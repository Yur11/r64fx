#include <iostream>
#include <iomanip>
#include "jit.h"

using namespace std;
using namespace r64fx;

typedef long int (*Fun)(void);

float a[8] = {1.0, 21.1234, 3.0, 4.0, 5.0, 6.1, 7.0, 8.0};
float b[4] = {1.1, 2.2, 3.3, 4.4};
float c[4] = {0.0, 0.0, 0.0, 0.0};

int d[4] = { -1, -1, -133, -1 };

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

    
    as.mov(rax, Imm64(d));
    as.cvtdq2ps(xmm0, Base(rax));
    as.movaps(Mem128(b), xmm0);
    
    as.ret();
    
    Fun fun = (Fun) as.getFun();
    
    dump(a);
    dump(b);
    dump(c);
    dump(d);
    
    cout << "---------\n";
    
    fun();
    
    dump(a);
    dump(b);
    dump(c);
    dump(d);
    
    return 0;
}