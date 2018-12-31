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

typedef DumpVec<float, 4> DumpF4;
typedef DumpVec<float, 8> DumpF8;
typedef DumpVec<float, 12> DumpF12;


float a[12] = {0.1f, 0.1f, 0.1f, 0.1f, 0.2f, 0.2f, 0.2f, 0.2f, 0.3f, 0.3f, 0.3f, 0.3f};
float b[12] = {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f};
float c[12] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

int main(int argc, char** argv)
{
    JitProcedures jp;
    JitProcSequence seq;
    seq.add(jp.procGain(), a+8, b+8, c+8, 12);
    seq.add(jp.procExit());

    cout << DumpF12(a) << "\n";
    cout << DumpF12(b) << "\n";
    cout << DumpF12(c) << "\n";
    cout << seq.exec(&jp, 1000) << "\n";
    cout << DumpF12(a) << "\n";
    cout << DumpF12(b) << "\n";
    cout << DumpF12(c) << "\n";

    return 0;
}
