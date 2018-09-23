#include "test.hpp"
#include "jit_procedures.hpp"

using namespace std;
using namespace r64fx;

template<typename T> inline void dump(T* v)
{
    cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ", "
         << v[4] << ", " << v[5] << ", " << v[6] << ", " << v[7] << "\n";
};

int playheads[32] = {
    /* value */  0,  1, 2, 3,
    /* delta */  1,  1,  1,  1,
    /* last  */  7, 15, 23, 31,
    /* first */  -1,  7, 15, 23,

    /* value */  4,  5, 6, 7,
    /* delta */  1,  1,  1,  1,
    /* last  */  7, 15, 23, 31,
    /* first */  -1,  7, 15, 23
};

float buff[32] = {
    88, 11, 22, 33, 44, 55, 66, 77,
    88, 11, 22, 33, 44, 55, 66, 77,
    88, 11, 22, 33, 44, 55, 66, 77,
    88, 11, 22, 33, 44, 55, 66, 77
};

float out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

int main(int argc, char** argv)
{
    JitProc_Playback<JitProcCommon> jit;
    JitProcSequence seq;
    add(seq, jit.proc_Playback(), playheads, buff, out, 8);
    seq.store((long)jit.procSeqExit());

    for(int i=0; i<16; i++)
    {
        jit.exec(seq, 1);
        dump(out);
    }


    return 0;
}
