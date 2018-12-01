#include "test.hpp"
#include "jit_procedures.hpp"
#include "Mixin.hpp"

using namespace std;
using namespace r64fx;

float a[8] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float b[8] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float c[8] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

template<typename T, long N> class Vec{
    T m[N];

public:
    Vec() { for(long i=0; i<N; i++) m[i] = T(); }
};

int main(int argc, char** argv)
{
    JitProcedures jp;
    JitProcSequence seq;
    seq.add(jp.procGain(), a, b, c, -20);
    seq.add(jp.procExit());
    cout << seq.exec(&jp, 1) << "\n";

    return 0;
}
