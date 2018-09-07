#include "test.hpp"
#include "jit_procedures.hpp"

using namespace std;
using namespace r64fx;

float a[4] = { 1.0, 2.0, 3.0, 4.0 };
float b[4] = { 1.0, 0.2, 0.5, 2.0 };
float c[4] = { 0.0, 0.0, 0.0, 0.0 };

inline void dump(float v[4])
{
    cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";
}

int main(int argc, char** argv)
{
    JitProc_Gain<JitProcCommon> jit;

    JitProcSequence seq;
    add(seq, jit.proc_Gain(), a, b, c, 4);
    seq.store((long)jit.procSeqExit());

    dump(a);
    dump(b);
    dump(c);
    cout << jit.exec(seq, 256) << "\n";
    dump(a);
    dump(b);
    dump(c);

    return 0;
}
