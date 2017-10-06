#include <iostream>
#include "SignalGraph.hpp"
#include "SignalNode_BufferRW.hpp"

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

constexpr int frame_count = 1024;
float buff_a[frame_count];
float buff_b[frame_count];

int main()
{
    srand(time(0));
    for(int i=0; i<frame_count; i++)
    {
        buff_a[i] = float(rand() & 0xFFFF) * 0.01f;
        buff_b[i] = 0.0f;
    }

    SignalGraph sg;
    sg.setFrameCount(frame_count);

    SignalNode_BufferReader snbr;
    SignalNode_BufferWriter snbw;

    sg.addNode(&snbr);
    sg.addNode(&snbw);
    sg.connect(snbr.source(), snbw.sink());

    SignalGraphProcessor sgp;
    sgp.build(sg);
    sgp.run();

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
