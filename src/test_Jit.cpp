#include <iostream>
#include "Jit.hpp"

using namespace std;
using namespace r64fx;

float buffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 17, 18, 19, 20, 21 };

int indices[4] = { 3, 11, 9, 11 };

int mask[4] = { -1, -1, -1, -1 };

class TestJit : public Assembler{
    JumpLabel8 l;

public:
    int run()
    {
        MOV       (rax, Addr(buffer));
        ADD       (rax, Imm8(8));
        MOVAPS    (xmm0, Mem128(indices));
        MOVAPS    (xmm1, Mem128(mask));

        VGATHERDPS (xmm2, Base(rax) + IndexXmm(xmm0) * Scale4, xmm1);

        MOVAPS    (Mem128(buffer), xmm2);

        RET       ();

        auto fun = (unsigned long (*)()) begin();

//         auto p = begin();
//         for(;;)
//         {
//             cout << hex << int(*p) << "\n";
// 
//             if(*p == 0xC3)
//                 break;
// 
//             p++;
//         }

        cout << dec << fun() << "\n";

        for (int i=0; i<16; i++)
        {
            cout << i << " -> " << buffer[i] << "\n";
        }

        return 0;
    }
};

int main()
{
    TestJit tj;
    return tj.run();
}
