#include <iostream>
#include "SignalGraph.hpp"
#include "SignalNode_BufferRW.hpp"
#include "SignalNode_Oscillator.hpp"

using namespace std;
using namespace r64fx;

bool buff_eq(float* a, float* b, int size)
{
    for(int i=0; i<size; i++)
    {
        if(a[i] != b[i])
            return false;
    }
    return true;
}

constexpr int frame_count = 8;
float buff_a[frame_count];
float buff_b[frame_count];

int main()
{
    cout << sizeof(SignalSource) << "\n";
    cout << sizeof(SignalSink) << "\n";

    srand(time(0));

    SignalGraphCompiler sgc;
    sgc.setFrameCount(frame_count);

//     SignalNode_OscClock clk;
//     clk.setDelta(0xFFFFFFF);
// 
//     SignalNode_BufferWriter bw;
//     bw.setBuffer(buff_a);
// 
//     sgc.link(clk.out(), bw.in());
// 
//     sgc.build(&bw, 1);
//     sgc.run();
// 
//     for(int i=0; i<frame_count; i++)
//         cout << buff_a[i] << "\n";
// 
//     return 0;

    SignalNode_BufferReader snbr;
    SignalNode_BufferWriter snbw;
    snbr.setBuffer(buff_a);
    snbw.setBuffer(buff_b);


    for(int i=0; i<frame_count; i++)
    {
        snbr.buffer(i) = float(rand() & 0xFFFF) * 0.01f;
        snbw.buffer(i) = 0.0f;
    }

    sgc.link(snbr.out(), snbw.in());

    sgc.build(&snbw, 1);
    sgc.run();

    for(int i=0; i<frame_count; i++)
    {
        cout << buff_a[i] << ", " << buff_b[i] << "\n";
    }

    if(buff_eq(buff_a, buff_b, frame_count))
    {
        cout << "OK\n";
        return 0;
    }
    else
    {
        cout << "Fail!\n";
        return 1;
    }
}
