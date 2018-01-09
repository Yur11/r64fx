#include <iostream>
#include <string>
#include <cmath>
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "Thread.hpp"

#define CLIENT_NAME "test_SoundDriver"

using namespace std;
using namespace r64fx;


namespace{

bool                     g_running          = true;
long                     g_frame_count      = 0;
long                     g_sample_rate      = 0;
float                    g_sample_rate_rcp  = 0.0f;

float                    g_volume           = 0.3f;
int                      g_volume_cc        = 102;

float                    g_even_partials    = 1.0f;
int                      g_even_partials_cc = 103;

unsigned long            g_sequence         = 0;
unsigned long            g_sequence_time    = 0;

SoundDriverPortGroup*    g_port_group       = nullptr;
SoundDriverAudioOutput*  g_synth_output     = nullptr;
SoundDriverAudioInput*   g_audio_input      = nullptr;
SoundDriverAudioOutput*  g_audio_output     = nullptr;
SoundDriverMidiInput*    g_midi_input       = nullptr;
SoundDriverMidiOutput*   g_midi_output      = nullptr;

constexpr int    PARTIAL_COUNT  = 32;
constexpr int    VOICE_COUNT    = 8;
constexpr float  INT_RCP        = 1.0f / int(0x7FFFFFFF);
constexpr float  PI_INT_RCP     = INT_RCP * 3.1415926535897932384626;
constexpr float  RCP_12         = 1.0f / 12.0f;
constexpr float  RCP_128        = 1.0f / 128.0f;


class Voice{
    struct Partial{
        int    value      = 0;
        int    increment  = 0;
        float  level      = 1.0f;
    };
    Partial  m_partial[PARTIAL_COUNT];
    int      m_note_number  = -1;
    float    m_freq         = 0.0f;

public:
    inline int noteNumber() const { return m_note_number; }

    inline bool on() const { return m_note_number > 0; }

    inline void noteOn(int note, int velocity)
    {
        m_note_number = note;
        m_freq = pow(2, (m_note_number - 69) * RCP_12) * 440.0f;
        for(int j=0; j<PARTIAL_COUNT; j++)
        {
            float freq = m_freq * (j+1);
            if(freq <= (g_sample_rate * 0.5f))
            {
                m_partial[j].increment = m_freq * g_sample_rate_rcp * 0xFFFFFFFFU * (j+1);
                m_partial[j].level = 1.0f / (j+1);
            }
            else
            {
                m_partial[j].increment = 0;
                m_partial[j].level = 0.0f;
            }
        }
    }

    inline void noteOff()
    {
        m_note_number = -1;
    }

    inline float process(int frame)
    {
        float voiceval = 0.0;
        for(int p=0; p<PARTIAL_COUNT && m_partial[p].increment; p++)
        {
            voiceval += sin(float(m_partial[p].value) * PI_INT_RCP) * m_partial[p].level * (p & 1 ? g_even_partials : 1.0f);
            m_partial[p].value += m_partial[p].increment;
        }
        return voiceval;
    }
};


void* worker_thread(void* arg)
{
    Voice voice[VOICE_COUNT];

    cout << g_frame_count << " frames @ " << g_sample_rate << "Hz\n";
    g_port_group->enable();

    auto synth_buffer = new float[g_frame_count];
    g_port_group->updatePort(g_synth_output, synth_buffer);

    auto audio_buffer = new float[g_frame_count];
    g_port_group->updatePort(g_audio_input, audio_buffer);
    g_port_group->updatePort(g_audio_output, audio_buffer);

    MidiEventBuffer midi_input_buffer;
    g_port_group->updatePort(g_midi_input, &midi_input_buffer);

    unsigned int buff_storage[32];
    MidiEventBuffer midi_output_buffer;
    g_port_group->updatePort(g_midi_output, &midi_output_buffer);

    while(g_running)
    {
        if(g_port_group->sync())
        {
            midi_output_buffer = MidiEventBuffer();

            MidiEvent event;
            while(midi_input_buffer.read(event))
            {
                cout << event << "\n";

                if(event.type() == MidiMessage::Type::NoteOn)
                {
                    for(int v=0; v<VOICE_COUNT; v++)
                    {
                        if(!voice[v].on())
                        {
                            voice[v].noteOn(event.noteNumber(), event.velocity());
                            break;
                        }
                    }
                }
                else if(event.type() == MidiMessage::Type::NoteOff)
                {
                    for(int v=0; v<VOICE_COUNT; v++)
                    {
                        if(voice[v].noteNumber() == event.noteNumber())
                        {
                            voice[v].noteOff();
                            break;
                        }
                    }
                }
                else if(event.type() == MidiMessage::Type::ControlChange)
                {
                    if(event.controllerNumber() == g_volume_cc)
                    {
                        g_volume = event.controllerValue() * 0.5f * RCP_128;
                    }
                    else if(event.controllerNumber() == g_even_partials_cc)
                    {
                        g_even_partials = event.controllerValue() * RCP_128;
                    }
                }
            }

            /* Synth Audio */
            for(long i=0; i<g_frame_count; i++)
            {
                float mix = 0.0;
                for(int v=0; v<VOICE_COUNT; v++)
                {
                    if(voice[v].on())
                    {
                        mix += voice[v].process(i) * g_volume;
                    }
                }
                synth_buffer[i] = mix;
            }

            /* Output Midi Sequence */
            g_sequence_time += 8;
            if(g_sequence_time >= 0xFF)
            {
                g_sequence_time = 0;

                unsigned int note[4] = { 53, 60, 67, 60 };

                midi_output_buffer = MidiEventBuffer(buff_storage, buff_storage + 32);
                auto begin = midi_output_buffer.ptr();
                if(g_sequence & 1)
                {
                    midi_output_buffer.write(MidiMessage::NoteOff(0, note[g_sequence>>1], 0));
                }
                else
                {
                    midi_output_buffer.write(MidiMessage::NoteOn(0, note[g_sequence>>1], 63));
                }

                midi_output_buffer = MidiEventBuffer(begin, midi_output_buffer.ptr());

                g_sequence++;
                g_sequence = g_sequence & 7;
            }

            g_port_group->done();
        }
        else
        {
            sleep_nanoseconds(1 * 1000 * 1000);
        }
    }

    g_port_group->updatePort(g_synth_output,  nullptr);
    g_port_group->updatePort(g_audio_input,   nullptr);
    g_port_group->updatePort(g_audio_output,  nullptr);
    g_port_group->updatePort(g_midi_input,    nullptr);
    g_port_group->updatePort(g_midi_output,   nullptr);

    for(int i=0; i<10; i++)
    {
        while(!g_port_group->sync())
            sleep_nanoseconds(5 * 1000 * 1000);
        g_port_group->done();
    }

    g_port_group->disable();
    delete[] audio_buffer;
    delete[] synth_buffer;
    return nullptr;
}

}//namespace


int main()
{
    auto sd = SoundDriver::newInstance(SoundDriver::Type::Jack, CLIENT_NAME);
    sd->enable();

    g_port_group    = sd->newPortGroup();
    g_synth_output  = sd->newAudioOutput("synth");
    g_audio_input   = sd->newAudioInput("in");
    g_audio_output  = sd->newAudioOutput("out");
    g_midi_input    = sd->newMidiInput("midi_in");
    g_midi_output   = sd->newMidiOutput("midi_out");

    g_frame_count = sd->bufferSize();
    g_sample_rate = sd->sampleRate();
    g_sample_rate_rcp = 1.0f / float(g_sample_rate);

    sd->connect("system:capture_1",      CLIENT_NAME":in");
    sd->connect("system:capture_2",      CLIENT_NAME":in");
    sd->connect(CLIENT_NAME":synth",     CLIENT_NAME":in");
    sd->connect(CLIENT_NAME":out",       "system:playback_1");
    sd->connect(CLIENT_NAME":out",       "system:playback_2");
    sd->connect(CLIENT_NAME":midi_out",  CLIENT_NAME":midi_in");

    /* Connect to Your midi stuff here. */
//     sd->connect("alsa_midi:A-PRO 1 (out)", "r64fx:midi_in");
//     sd->connect("alsa_midi:A-PRO 2 (out)", "r64fx:midi_in");
//     sd->connect("alsa_midi:A-PRO MIDI (out)", "r64fx:midi_in");
//     sd->connect("alsa_midi:Akai MPD32 MIDI 1 (out)", "r64fx:midi_in");
//     sd->connect("alsa_midi:Akai MPD32 MIDI 2 (out)", "r64fx:midi_in");
//     sd->connect("alsa_midi:Akai MPD32 MIDI 3 (out)", "r64fx:midi_in");
//     sd->connect("alsa_midi:LPD8 MIDI 1 (out)", "r64fx:midi_in");

    Thread wt;
    wt.run(worker_thread);

    while(g_running)
    {
        string str;
        cin >> str;
        if(str == "q" || str == "quit" || str == "exit" || str == "die")
        {
            g_running = false;
        }
        else
        {
            sleep_nanoseconds(300 * 1000 * 1000);
        }
    }

    wt.join();

    sd->deletePort(g_synth_output);
    sd->deletePort(g_audio_input);
    sd->deletePort(g_audio_output);
    sd->deletePort(g_midi_input);
    sd->deletePort(g_midi_output);
    sd->deletePortGroup(g_port_group);

    while(!sd->isClean())
    {
        sd->sync();
        sleep_nanoseconds(300 * 1000 * 1000);
    }

    sd->disable();
    SoundDriver::deleteInstance(sd);
    return 0;
}
