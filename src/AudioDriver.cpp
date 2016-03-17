#include "AudioDriver.hpp"
#include <jack/jack.h>
#include <jack/midiport.h>
#include <cstring>
#include <new>
#include <vector>
#include <iostream>

using namespace std;

namespace r64fx{

struct IOPortJack{
    jack_port_t* jack_port     = nullptr;
    bool         is_input      = false;
    int          attempt_count = 0;
};


struct AudioIOPort_Jack : public AudioIOPort, public IOPortJack{
    float* buffer = nullptr;

    virtual ~AudioIOPort_Jack(){}

    virtual float* samples()
    {
        return buffer;
    }

    virtual bool isInput()
    {
        return is_input;
    }

    virtual bool isGood()
    {
        return buffer != nullptr && jack_port != nullptr;
    }
};


namespace{
    constexpr int max_midi_event_count = 24;
};

struct MidiIOPort_Jack : public MidiIOPort, public IOPortJack{
    MidiEvent events[max_midi_event_count];
    int event_count = 0;

    virtual ~MidiIOPort_Jack(){}

    virtual MidiEvent* event(int i)
    {
        return events + i;
    }

    virtual int eventCount()
    {
        return event_count;
    }
};


struct AudioDriver_Jack : public AudioDriver{
    jack_client_t* m_jack_client = nullptr;
    vector<AudioIOPort_Jack*> m_audio_ports;
    vector<MidiIOPort_Jack*>  m_midi_ports;

    unsigned long m_count = 0;
    Mutex         m_count_mutex;

    AudioDriver_Jack()
    {
        m_jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        jack_set_error_function([](const char* message){
            cout << "Jack Error: " << message << "\n";
        });

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg) -> int {
            auto self = (AudioDriver_Jack*) arg;
            return self->process(nframes);
        }, this) != 0)
        {
            jack_client_close(m_jack_client);
        }
    }


    int process(int nframes)
    {
        for(auto port : m_audio_ports)
        {
            port->attempt_count = 0;
        }

        for(auto port : m_midi_ports)
        {
            port->attempt_count = 0;
            port->event_count = 0;
        }

        static const int max_attemt_count = 2;

        int midi_ports_to_process  = m_midi_ports.size();
        int midi_ports_skipped     = 0;
        while(midi_ports_to_process)
        {
            for(auto port : m_midi_ports)
            {
                if(port->tryLock())
                {
                    void* port_buff = jack_port_get_buffer(
                        port->jack_port, nframes
                    );

                    if(port->is_input)
                    {
                        int nevents = jack_midi_get_event_count(port_buff);
                        for(int i=0; i<nevents; i++)
                        {
                            jack_midi_event_t event;
                            jack_midi_event_get(&event, port_buff, i);

                            if(event.size <= 3)
                            {
                                MidiEvent* out_event = nullptr;
                                if(port->event_count < max_midi_event_count)
                                {
                                    out_event = port->event(port->event_count);
                                    port->event_count++;
                                }
                                else
                                {
                                    out_event = port->event(max_midi_event_count - 1);
                                }

                                out_event[0] = MidiEvent(
                                    MidiMessage(event.buffer, event.size), event.time
                                );
                            }
                        }
                    }
                    else
                    {

                    }
                    port->unlock();
                    midi_ports_to_process--;
                }
                else
                {
                    if(port->attempt_count >= max_attemt_count)
                    {
                        midi_ports_skipped++;
                        midi_ports_to_process--;
                    }
                    else
                    {
                        port->attempt_count++;
                    }
                }
            }
        }

        int audio_ports_to_process = m_audio_ports.size();
        int audio_ports_skipped    = 0;
        while(audio_ports_to_process)
        {
            for(auto port : m_audio_ports)
            {
                if(port->tryLock())
                {
                    void* port_buff = jack_port_get_buffer(
                        port->jack_port,
                        nframes
                    );

                    if(port->is_input)
                    {
                        memcpy(port->buffer, port_buff, nframes * sizeof(float));
                    }
                    else
                    {
                        memcpy(port_buff, port->buffer, nframes * sizeof(float));
                    }
                    port->unlock();
                    audio_ports_to_process--;
                }
                else
                {
                    if(port->attempt_count >= max_attemt_count)
                    {
                        audio_ports_skipped++;
                        audio_ports_to_process--;
                    }
                    else
                    {
                        port->attempt_count++;
                    }
                }
            }
        }

        if(midi_ports_skipped)
        {
            cout << "Skipped Midi Ports: " << midi_ports_skipped << "!\n";
        }

        if(audio_ports_skipped)
        {
            cout << "Skipped Audio Ports: " << audio_ports_skipped << " !\n";
        }

        int attempt_count = 0;
        while(attempt_count < 10)
        {
            attempt_count++;
            if(m_count_mutex.tryLock())
            {
                m_count++;
                m_count_mutex.unlock();
                break;
            }
        }

        if(attempt_count > 1)
        {
            cout << "Attempt Count: " << attempt_count << "\n";
        }

        return 0;
    }


    virtual ~AudioDriver_Jack()
    {
//         if(m_jack_client)
//         {
//             jack_client_close(m_jack_client);
//         }
    }


    virtual unsigned long count()
    {
        m_count_mutex.lock();
        unsigned long c = m_count;
        m_count_mutex.unlock();
        return c;
    }


    virtual bool isGood()
    {
        return m_jack_client != nullptr;
    }


    virtual void enable()
    {
        jack_activate(m_jack_client);
    }


    virtual void disable()
    {
        jack_deactivate(m_jack_client);
    }


    virtual int bufferSize()
    {
        return jack_get_buffer_size(m_jack_client);
    }


    virtual int sampleRate()
    {
        return jack_get_sample_rate(m_jack_client);
    }


    virtual AudioIOPort* newAudioInputPort(const char* name)
    {
        return newAudioPort(name, true);
    }


    virtual AudioIOPort* newAudioOutputPort(const char* name)
    {
        return newAudioPort(name, false);
    }


    AudioIOPort* newAudioPort(const char* name, bool is_input)
    {
        auto port = new(std::nothrow) AudioIOPort_Jack;
        if(!port)
            return port;

        port->jack_port = jack_port_register(
            m_jack_client,
            name,
            JACK_DEFAULT_AUDIO_TYPE,
            is_input ? JackPortIsInput : JackPortIsOutput,
            0
        );
        if(!port->jack_port)
        {
            cerr << "Failed to create jack port \"" << name << "\"!\n";
            delete port;
            return nullptr;
        }

        port->buffer = new(std::nothrow) float[bufferSize()];
        if(!port->buffer)
        {
            cerr << "Failed to create buffer for port \"" << name << "\"!\n";
            jack_port_unregister(m_jack_client, port->jack_port);
            delete port;
            return nullptr;
        }

        for(int i=0; i<bufferSize(); i++)
        {
            port->buffer[i] = 0.0f;
        }

        port->is_input = is_input;

        m_audio_ports.push_back(port);
        return port;
    }


    virtual void deleteAudioPort(AudioIOPort* port)
    {
        auto it = m_audio_ports.begin();
        while(it != m_audio_ports.end() || *it != port) it++;
        if(it != m_audio_ports.end())
        {
            m_audio_ports.erase(it);
            AudioIOPort_Jack* port = *it;
            if(port->buffer)
                delete[] port->buffer;
            if(port->jack_port)
                jack_port_unregister(m_jack_client, port->jack_port);
            delete port;
        }
    }


    virtual MidiIOPort* newMidiInputPort(const char* name)
    {
        return newMidiPort(name, true);
    }


    virtual MidiIOPort* newMidiOutputPort(const char* name)
    {
        return newMidiPort(name, false);
    }


    MidiIOPort* newMidiPort(const char* name, bool is_input)
    {
        auto port = new(std::nothrow) MidiIOPort_Jack;
        if(!port)
            return port;

        port->jack_port = jack_port_register(
            m_jack_client,
            name,
            JACK_DEFAULT_MIDI_TYPE,
            is_input ? JackPortIsInput : JackPortIsOutput,
            0
        );
        if(!port->jack_port)
        {
            cerr << "Failed to create jack port \"" << name << "\"!\n";
            delete port;
            return nullptr;
        }

        port->is_input = is_input;

        m_midi_ports.push_back(port);
        return port;
    }


    virtual void deleteMidiPort(MidiIOPort* port)
    {
        auto it = m_midi_ports.begin();
        while(it != m_midi_ports.end() || *it != port) it++;
        if(it != m_midi_ports.end())
        {
            m_midi_ports.erase(it);
            MidiIOPort_Jack* port = *it;
            if(port->jack_port)
                jack_port_unregister(m_jack_client, port->jack_port);
            delete *it;
        }
    }
};


AudioDriver* AudioDriver::newInstance(AudioDriver::Type type)
{
    AudioDriver* driver = nullptr;
    switch(type)
    {
        case AudioDriver::Type::Jack:
        {
            driver = new(std::nothrow) AudioDriver_Jack;
            break;
        }

        default:
        {
            break;
        }
    }

    if(driver && !driver->isGood())
    {
        deleteInstance(driver);
        driver = nullptr;
    }
    return driver;
}


void AudioDriver::deleteInstance(AudioDriver* driver)
{
    delete driver;
}

}//namespace r64fx