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

constexpr int frame_count = 8;
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

    SignalNode_BufferReader snbr; snbr.setBuffer(buff_a);
    SignalNode_BufferWriter snbw; snbw.setBuffer(buff_b);

    sg.addNode(&snbr);
    sg.addNode(&snbw);
    sg.connect(snbr.source(), snbw.sink());

    SignalGraphProcessor sgp;
    sgp.build(sg, frame_count);
    sgp.run();

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
