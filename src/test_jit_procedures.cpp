#include "test.hpp"
#include "jit_procedures.hpp"

using namespace std;
using namespace r64fx;

float a[4] = { 1.0, 2.0, 3.0, 4.0 };
float b[4] = { 1.0, 0.2, 0.5, 2.0 };
float c[4] = { 0.0, 0.0, 0.0, 0.0 };

inline void dump(float* v)
{
    cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";
};

int playheads[16] = {
    /* value */  0,  9, 16, 26,
    /* delta */  1,  1,  1,  1,
    /* last  */  7, 15, 23, 31,
    /* first */  -1,  7, 15, 23
};

float buff[32] = {
    11, 1, 2, 3, 4, 5, 6, 7,
    22, 1, 2, 3, 4, 5, 6, 7,
    33, 1, 2, 3, 4, 5, 6, 7,
    44, 1, 2, 3, 4, 5, 6, 7
};

float out[4] = { 0, 0, 0, 0 };

int main(int argc, char** argv)
{
//     JitProc_Gain<JitProcCommon> jit;
// 
//     JitProcSequence seq;
//     add(seq, jit.proc_Gain(), a, b, c, 4);
//     seq.store((long)jit.procSeqExit());
// 
//     dump(a);
//     dump(b);
//     dump(c);
//     cout << jit.exec(seq, 256) << "\n";
//     dump(a);
//     dump(b);
//     dump(c);

    JitProc_Playback<JitProcCommon> jit;
    JitProcSequence seq;
    add(seq, jit.proc_Playback(), playheads, buff, out, 4);
    seq.store((long)jit.procSeqExit());

    dump(out);
    for(int i=0; i<16; i++)
    {
        jit.exec(seq, 1);
        dump(out);
    }


    return 0;
}
