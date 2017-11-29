#include "test.hpp"
#include "SignalGraph.hpp"
#include "SignalNode_BufferRW.hpp"
#include "SignalNode_Oscillator.hpp"

using namespace std;
using namespace r64fx;


bool test_BufferRW(SignalGraph &sg)
{
    constexpr int frame_count = 8;
    float buff_a[frame_count];
    float buff_b[frame_count];
    for(int i=0; i<frame_count; i++)
    {
        buff_a[i] = float(rand() & 0xFFFF) * 0.01f;
        buff_b[i] = 0.0f;
    }
    sg.setFrameCount(frame_count);

    SignalNode_BufferReader snbr(sg, buff_a);
    SignalNode_BufferWriter snbw(sg, buff_b);

    sg.link(snbr.out(), snbw.in());
    sg.build(&snbw, 1);
    sg.run();
    R64FX_EXPECT_VEC_EQ(buff_a, buff_b, frame_count);

    return true;
}


int main()
{
    srand(time(0));

    SignalGraph sg;

    bool ok = test_BufferRW(sg);

    if(ok)
    {
        std::cout << "OK!\n";
        return 0;
    }
    else
    {
        std::cout << "Fail!\n";
        return 1;
    }
}
