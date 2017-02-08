#include <iostream>
#include <string>
#include <cmath>
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"

using namespace std;
using namespace r64fx;

bool g_running = true;
Mutex g_mutex;
SoundDriverSyncPort* g_sync_port = nullptr;

int main()
{
    auto sd = SoundDriver::newInstance(SoundDriver::Type::Jack);
    sd->enable();

    g_mutex.lock();
    g_sync_port    = sd->newSyncPort();
    auto audio_in  = sd->newAudioInput  ("in");
    auto audio_out = sd->newAudioOutput ("out");
    auto midi_in   = sd->newMidiInput   ("midi_in");
    auto midi_out  = sd->newMidiOutput  ("midi_out");
    sd->connect("r64fx:out", "system:playback_1");
    sd->connect("r64fx:out", "system:playback_2");
    sd->connect("alsa_midi:Midi Through Port-0 (out)", "r64fx:midi_in");
    g_mutex.unlock();

    Thread cmd_thread;
    cmd_thread.run([](void*) -> void*{
        while(g_running)
        {
            string cmd = "";
            cin >> cmd;
            if(cmd == "quit" || cmd == "exit" || cmd == "done")
            {
                g_running = false;
                break;
            }
            else if(cmd == "enable")
            {
                g_mutex.lock();
                g_sync_port->enable();
                g_mutex.unlock();
            }
            else if(cmd == "disable")
            {
                g_mutex.lock();
                g_sync_port->disable();
                g_mutex.unlock();
            }
        }
        return nullptr;
    }, nullptr);

    const int max_partials  = 32;
    int     npartials       = max_partials;
    float   npartials_rcp   = 1.0f / float(npartials);
    float   osc[max_partials];
    for(int i=0; i<max_partials; i++)
    {
        osc[i] = 0.0f;
    }
    float   freq            = 440.0f;
    float   nyquist         = float(sd->sampleRate() / 2);
    float   srrcp           = 1.0f / float(sd->sampleRate());
    float*  buff            = new float[sd->bufferSize()];
    long    note_period     = sd->sampleRate();
    long    sample_count    = 0;
    bool    note_on         = false;

    g_mutex.lock();
    g_sync_port->enable();
    g_mutex.unlock();
    while(g_running)
    {
        g_mutex.lock();
        sd->processEvents();

        SoundDriverSyncMessage sync_msg;
        if(g_sync_port->readMessages(&sync_msg, 1) == 1)
        {
            MidiEvent midi_event;
            while(midi_in->readEvents(&midi_event, 1))
            {
                MidiMessage msg = midi_event.message();
                switch(msg.type())
                {
                    case MidiMessage::Type::NoteOn:
                    {
                        cout << "NoteOn:  "  << msg.channel() << ", note: " << msg.noteNumber() << ", vel: " << msg.velocity() << "\n";
                        freq = pow(2, (msg.noteNumber() - 69) / 12.0f) * 440.0f;
                        break;
                    }

                    case MidiMessage::Type::NoteOff:
                    {
                        cout << "NoteOff: "  << msg.channel() << ", note: " << msg.noteNumber() << ", vel: " << msg.velocity() << "\n";
                        break;
                    }

                    case MidiMessage::Type::ControlChange:
                    {
                        cout << "CC: " << msg.channel() << ", " << msg.controllerNumber() << ", " << msg.controllerValue() << "\n";
                        if(msg.controllerNumber() == 19)
                        {
                            auto val = (msg.controllerValue() >> 2) + 1;
                            cout << "val: " << val << "\n";
                            npartials = val;
                        }
                    }

                    default:
                    {
                        break;
                    }
                }
            }

            sample_count += sd->bufferSize();
            if(sample_count >= note_period)
            {
                sample_count -= note_period;
                if(note_on)
                {
                    MidiEvent event(MidiMessage::NoteOff(0, 69, 127), 1);
                    midi_out->writeEvents(&event, 1);
                }
                else
                {
                    MidiEvent event(MidiMessage::NoteOn(0, 69, 0), 1);
                    midi_out->writeEvents(&event, 1);
                }
                note_on = !note_on;
            }

            audio_in->readSamples(buff, sd->bufferSize());
            if(freq > 0)
            {
                for(int i=0; i<sd->bufferSize(); i++)
                {
                    for(int j=0; j<npartials; j++)
                    {
                        osc[j] += freq * srrcp * float(j + 1);
                        if(osc[j] >= 1.0f)
                        {
                            osc[j] -= 2.0f;
                        }

                        if(freq <= nyquist)
                        {
                            buff[i] += sin(osc[j] * M_PI) * (1.0f/float(j + 1)) * npartials_rcp;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
            audio_out->writeSamples(buff, sd->bufferSize());
        }
        g_mutex.unlock();

        sleep_nanoseconds(1000 * 1000);
    }

    if(note_on)
    {
        MidiEvent event(MidiMessage::NoteOff(0, 69, 0), 0);
        midi_out->writeEvents(&event, 1);
    }

    delete[] buff;
    sd->deletePort(audio_in);
    sd->deletePort(audio_out);
    sd->deletePort(midi_in);
    sd->deletePort(midi_out);
    sd->deleteSyncPort(g_sync_port);
    while(sd->hasPorts())
    {
        sd->processEvents();
        sleep_nanoseconds(1000 * 1000);
    }
    sd->disable();
    SoundDriver::deleteInstance(sd);
    cmd_thread.join();
    cout << "Done!\n";
    return 0;
}
