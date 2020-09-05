#include <iostream>
#include "Jit.hpp"

using namespace std;
using namespace r64fx;

struct TestJit : public Assembler{
    TestJit() { rewind(); }

    inline long fl() { return ((long(*)()) begin())(); }
};


struct TestJit_Basic
    : public TestJit{
    bool run()
    {
        long num = 12345;
        MOV(rax, Imm64(num));
        RET();
        return fl() == num;
    }
};


struct TestJit_Jumps
    : public TestJit{
    bool run(bool long_jump)
    {
        MOV(rcx, Imm64(13));
        XOR(rax, rax);
        auto loop = ptr();
        ADD(rax, Imm8(2));
        SUB(rcx, Imm8(1));
        if(long_jump) for(int i=0; i<1024; i++) NOP();
        JNZ(Rel(loop));
        RET();

        return true;
    }
};

struct TestJit_JumpsShort : public TestJit_Jumps{
    bool run(){ TestJit_Jumps::run(false); }
};

struct TestJit_JumpsLong : public TestJit_Jumps{
    bool run(){ TestJit_Jumps::run(true); }
};


#define TEST_JIT(Test)\
    { cout << #Test " "; TestJit_##Test test; if(test.run()) { cout << "[OK]\n"; } \
        else { cout << "[FAIL]\n"; ok = false; } }

int main()
{
    bool ok = true;

    TEST_JIT(Basic);
    TEST_JIT(JumpsShort)
    TEST_JIT(JumpsLong)

    return ok ? 0 : 1;
}
