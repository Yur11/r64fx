#include "test.hpp"
#include "SignalGraph.hpp"
#include "SignalNode_BufferRW.hpp"
#include "SignalNode_Oscillator.hpp"

using namespace std;
using namespace r64fx;


bool test_BufferRW(SignalGraph &sg)
{
    cout << "test_BufferRW\n";

    constexpr int frame_count = 8;

    sg.setFrameCount(frame_count);
    float buff_a[frame_count];
    float buff_b[frame_count];
    for(int i=0; i<frame_count; i++)
    {
        buff_a[i] = float(rand() & 0xFFFF) * 0.01f;
        buff_b[i] = 0.0f;
    }

    SignalNode_BufferReader snbr(&sg, buff_a);
    SignalNode_BufferWriter snbw(&sg, buff_b);

    sg.link(snbr.out(), snbw.in());
    sg.build(&snbw);
    sg.run();
    R64FX_EXPECT_VEC_EQ(buff_a, buff_b, frame_count);

    return true;
}


bool test_BufferRW2(SignalGraph &sg)
{
    cout << "test_BufferRW2\n";

    constexpr int frame_count = 8;

    sg.setFrameCount(frame_count);
    float buff_a1[frame_count];
    float buff_a2[frame_count];
    float buff_b1[frame_count];
    float buff_b2[frame_count];
    for(int i=0; i<frame_count; i++)
    {
        buff_a1[i] = float(rand() & 0xFFFF) * 0.01f;
        buff_a2[i] = float(rand() & 0xFFFF) * 0.01f;
        buff_b1[i] = 0.0f;
        buff_b2[i] = 0.0f;
    }

    SignalNode_BufferReader snbr(&sg, buff_a1, buff_a2);
    SignalNode_BufferWriter snbw(&sg, buff_b1, buff_b2);

    sg.link(snbr.out(), snbw.in());
    sg.build(&snbw);
    sg.run();

    cout << "1\n"; R64FX_EXPECT_VEC_EQ(buff_a1, buff_b1, frame_count);
    cout << "2\n"; R64FX_EXPECT_VEC_EQ(buff_a2, buff_b2, frame_count);

    return true;
}


int main()
{
    srand(time(0));

    SignalGraph sg;

    bool ok = test_BufferRW(sg) && test_BufferRW2(sg);

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
