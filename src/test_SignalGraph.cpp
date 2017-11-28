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

    SignalGraph sg;
    sg.setFrameCount(frame_count);

    SignalNode_BufferReader snbr(sg);
    SignalNode_BufferWriter snbw(sg);
    snbr.setBuffer(buff_a);
    snbw.setBuffer(buff_b);


    for(int i=0; i<frame_count; i++)
    {
        snbr.buffer(i) = float(rand() & 0xFFFF) * 0.01f;
        snbw.buffer(i) = 0.0f;
    }

    sg.link(snbr.out(), snbw.in());

    sg.build(&snbw, 1);
    sg.run();

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
