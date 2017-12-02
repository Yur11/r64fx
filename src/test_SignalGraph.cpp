#include "test.hpp"
#include "SignalGraph.hpp"
#include "SignalNode_BufferRW.hpp"
#include "SignalNode_Oscillator.hpp"

using namespace std;
using namespace r64fx;


bool test_BufferRW(SignalGraph &sg)
{
    constexpr int frame_count = 8;

    sg.setFrameCount(frame_count);
    auto buff_a = sg.allocBuffer();
    auto buff_b = sg.allocBuffer();
    for(int i=0; i<frame_count; i++)
    {
        sg.addr(buff_a)[i] = float(rand() & 0xFFFF) * 0.01f;
        sg.addr(buff_b)[i] = 0.0f;
    }

    SignalNode_BufferReader snbr(sg, buff_a);
    SignalNode_BufferWriter snbw(sg, buff_b);

    sg.link(snbr.out(), snbw.in());
    sg.build(&snbw);
    sg.run();
    R64FX_EXPECT_VEC_EQ(sg.addr(buff_a), sg.addr(buff_b), frame_count);

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
