#include "test.hpp"
#include "jit_procedures.hpp"
#include "Mixin.hpp"

using namespace std;
using namespace r64fx;

template<typename T, long N> class DumpVec{
    T* m;

public:
    explicit DumpVec(T* p) : m(p) {}

    inline static long size() { return N; }

    inline T operator[](long i) const { return m[i]; }
};

template<class StreamT, class DumpVecT> StreamT &operator<<(StreamT &stream, const DumpVecT &dv)
{
    for(long i=0; i<dv.size(); i++)
        stream << dv[i] << (i == dv.size()-1 ? "" : ", ");
    return stream;
}

template<typename F> F fact(F f)
{
    if(f <= F(0))
        return F(1);
    return f * fact(f-F(1));
}

typedef DumpVec<float, 4> DumpF4;
typedef DumpVec<float, 8> DumpF8;
typedef DumpVec<float, 12> DumpF12;


float a[12] = {0.1f, 0.1f, 0.1f, 0.1f,   0.2f, 0.2f, 0.2f, 0.2f,   0.3f, 0.3f, 0.3f, 0.3f};
float b[12] = {1.0f, 2.0f, 3.0f, 4.0f,   1.0f, 2.0f, 3.0f, 4.0f,   1.0f, 2.0f, 3.0f, 4.0f};
float c[12] = {1.0f, 0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 1.0f, 0.0f};

int    clk      [4] = {0, 0, 0, 0};
int    clk_step [4] = {0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF};
float  out      [4] = {0.0f, 0.0f, 0.0f, 0.0f};

float  coeffs     [24] = {
    3.1415926535f / 0x7FFFFFFF,  3.1415926535f / 0x7FFFFFFF, 3.1415926535f / 0x7FFFFFFF, 3.1415926535f / 0x7FFFFFFF,
    1.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.5f,
    1.0f/fact(4), 1.0f/fact(4), 1.0f/fact(4), 1.0f/fact(4),
    1.0f/fact(6), 1.0f/fact(6), 1.0f/fact(6), 1.0f/fact(6),
    1.0f/fact(8), 1.0f/fact(8), 1.0f/fact(8), 1.0f/fact(8)
};

int main(int argc, char** argv)
{
    JitProc jp;
    JitProc::Sequence seq;

    seq << jp.procOscTaylorCosF4(1, clk, clk_step, out, coeffs)
        << jp.procExit();

    cout << seq.exec(&jp, 1024) / 1024.0f << "\n";

    return 0;
}
