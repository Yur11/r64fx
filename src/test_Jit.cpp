#include <iostream>
#include "Jit.hpp"

using namespace std;
using namespace r64fx;

float buffer[] = {
    0, 1, 2, 3,  4, 5, 6, 7,
    2, 2, 2, 2,  2, 2, 2, 2,
    3, 3, 3, 3,  3, 3, 3, 3,
    0, 0, 0, 0,  0, 0, 0, 0
};

class TestJit : public Assembler{
    JumpLabel8 l;

public:
    int run()
    {
        VMOVAPS (ymm0, Mem128(buffer));
        VMOVAPS (ymm1, Mem128(buffer + 8));
        VMOVAPS (ymm2, Mem128(buffer + 16));
        MOV(rdx, Addr(buffer + 16));
        XOR(rcx, rcx);

        VFMADD231PS  (ymm0, ymm1, Base(rdx) + Index(rcx) * Scale1);

        VMOVAPS (Mem128(buffer + 24),  ymm0);

        RET     ();

        auto fun = (long (*)()) begin();

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

        for(int i=0; i<32; i++)
        {
            if((i & 0x7) == 0)
                cout << "\n";
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
