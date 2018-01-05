#include "test.hpp"
#include "Midi.hpp"

using namespace std;
using namespace r64fx;

int main()
{
    constexpr unsigned int buff_size = 1024;
    unsigned int buff[buff_size];
    MidiEvent expected_buff[buff_size];
    srand(time(nullptr));

    for(int n=0; n<0xFF; n++)
    {
        MidiEventOutputBuffer meob(buff, buff+buff_size);

        unsigned int time = 0;
        unsigned int nwritten = 0;
        unsigned int nread = 0;
        for(unsigned int i=0; i<buff_size; i++)
        {
            unsigned int dt = rand() & 0x1FF;
            time += dt;

            MidiEvent event(MidiMessage((((unsigned int)rand()) & 0x00FFFFFF) | 0x80), time);

            if(!meob.write(event))
            {
                break;
            }
            expected_buff[i] = event;
            nwritten++;
        }

        MidiEventInputBuffer  meib(buff, meob.ptr());
        for(unsigned int i=0; i<buff_size; i++)
        {
            MidiEvent event;
            if(!meib.read(event))
            {
                break;
            }
            R64FX_EXPECT_EQ(expected_buff[i], event);
            nread++;
        }
        R64FX_EXPECT_EQ(nwritten, nread);
    }

    cout << "OK\n";
    return 0;
}
