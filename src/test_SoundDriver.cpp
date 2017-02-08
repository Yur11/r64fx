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
    g_sync_port = sd->newSyncPort();
    auto audio_out = sd->newAudioOutput("out");
    auto midi_in = sd->newMidiInput("in");
    g_mutex.unlock();

    Thread cmd_thread;
    cmd_thread.run([](void*) -> void*{
        for(;;)
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

    float freq1 = 441.0f;
    float freq2 = 439.0f;
    float osc1 = 0.0f;
    float osc2 = 0.0f;
    float srrcp = 1.0f / float(sd->sampleRate());
    float* buff = new float[sd->bufferSize()];
    long noteon_time = 0;
    long noteoff_time = 0;
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
                        noteon_time = sync_msg.bits * sd->bufferSize() + midi_event.time();
                        cout << "NoteOn:  "  << msg.channel() << ", note: " << msg.noteNumber() << ", vel: " << msg.velocity() << "\n";
                        break;
                    }

                    case MidiMessage::Type::NoteOff:
                    {
                        noteoff_time = sync_msg.bits * sd->bufferSize() + midi_event.time();
                        cout << "NoteOff: "  << msg.channel() << ", note: " << msg.noteNumber() << ", vel: " << msg.velocity() << "\n";
                        cout << "diff: " << (noteoff_time - noteon_time) << "\n";
                        break;
                    }

                    case MidiMessage::Type::ChanAft:
                    {
//                         cout << "ChanAft: " << msg.channel() << ", " << msg.channelPressure() << "\n";
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }

            for(int i=0; i<sd->bufferSize(); i++)
            {
                osc2 += freq2 * srrcp;
                if(osc2 >= 1.0f)
                {
                    osc2 -= 1.0f;
                }

                osc1 += freq1 * srrcp;
                if(osc1 >= 1.0f)
                {
                    osc1 -= 1.0f;
                }

                buff[i] = (sin((osc1 * 2.0f - 1.0f) * M_PI) + sin((osc2 * 2.0f - 1.0f) * M_PI)) * 0.35f;
            }
            audio_out->writeSamples(buff, sd->bufferSize());
        }
        g_mutex.unlock();

        sleep_nanoseconds(1000 * 1000);
    }

    delete[] buff;
    sd->deletePort(audio_out);
    sd->deletePort(midi_in);
    sd->deleteSyncPort(g_sync_port);
    sd->disable();
    SoundDriver::deleteInstance(sd);
    cmd_thread.join();
    cout << "Exit!\n";
    return 0;
}
