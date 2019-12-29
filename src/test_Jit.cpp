#include <iostream>
#include "Jit.hpp"

using namespace std;
using namespace r64fx;

float num[16] = { 1, 2, 3, 4,  5, 6, 7, 8,  0, 0, 0, 0,  0, 0, 0, 0 };

class TestJit : public Assembler{
    JumpLabel8 l;

public:
    int run()
    {
        MOV      (r9d, Imm32(3478));
        MOV      (rax, r9);

        RET      ();

        auto fun = (unsigned long (*)()) begin();

        auto p = begin();
        for(;;)
        {
            cout << hex << int(*p) << "\n";

            if(*p == 0xC3)
                break;

            p++;
        }

        cout << dec << fun() << "\n";

        for (int i=0; i<16; i++)
        {
            cout << i << " -> " << num[i] << "\n";
        }

        return 0;
    }
};

int main()
{
    TestJit tj;
    return tj.run();
}
