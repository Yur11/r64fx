#include "AudioDriver.hpp"
#include <jack/jack.h>
#include <pthread.h>
#include <cstring>
#include <new>
#include <vector>
#include <iostream>

using namespace std;

namespace r64fx{

struct AudioIOPort_Jack : public AudioIOPort{
    float*           buffer = nullptr;
    jack_port_t*     jack_port = nullptr;
    pthread_mutex_t  m_mutex = PTHREAD_MUTEX_INITIALIZER;
    bool             is_input = false;
    int              attempt_count = 0;

    AudioIOPort_Jack(jack_client_t* jack_client, const char* name, int nframes, bool is_input)
    : is_input(is_input)
    {
        jack_port = jack_port_register(
            jack_client,
            name,
            JACK_DEFAULT_AUDIO_TYPE,
            is_input ? JackPortIsInput : JackPortIsOutput,
            0
        );
        if(!jack_port)
        {
            cerr << "Failed to create jack port \"" << name << "\"!\n";
            return;
        }

        buffer = new(std::nothrow) float[nframes];
        if(!buffer)
        {
            jack_port_unregister(jack_client, jack_port);
            return;
        }

        for(int i=0; i<nframes; i++)
        {
            buffer[i] = 0.0f;
        }
    }

    virtual ~AudioIOPort_Jack()
    {
        if(buffer)
            delete[] buffer;
    }

    virtual float* samples()
    {
        return buffer;
    }

    virtual void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    virtual bool tryLock()
    {
        return pthread_mutex_trylock(&m_mutex) == 0;
    }

    virtual void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
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


struct AudioDriver_Jack : public AudioDriver{
    jack_client_t* m_jack_client = nullptr;
    vector<AudioIOPort_Jack*> m_audio_ports;

    AudioDriver_Jack()
    {
        m_jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg) -> int
        {
            auto self = (AudioDriver_Jack*) arg;
            auto &audio_ports = self->m_audio_ports;

            for(auto port : audio_ports)
            {
                port->attempt_count = 0;
            }

            static const int max_attemt_count = 2;
            int ports_to_process = audio_ports.size();
            int ports_skipped = 0;
            while(ports_to_process)
            {
                for(auto port : audio_ports)
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
                        ports_to_process--;
                    }
                    else
                    {
                        if(port->attempt_count >= max_attemt_count)
                        {
                            ports_skipped++;
                            ports_to_process--;
                        }
                        else
                        {
                            port->attempt_count++;
                        }
                    }
                }
            }

            if(ports_skipped)
            {
                cout << "Skipped " << ports_skipped << " !\n";
            }

            return 0;
        }, this) != 0)
        {
            jack_client_close(m_jack_client);
        }
    }

    virtual ~AudioDriver_Jack()
    {
        if(m_jack_client)
            jack_client_close(m_jack_client);
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
        auto port = new(std::nothrow) AudioIOPort_Jack(m_jack_client, name, bufferSize(), is_input);
        if(!port)
            return port;
        m_audio_ports.push_back(port);
        return port;
    }

    virtual void deleteAudioPort(AudioIOPort* port)
    {
        auto it = m_audio_ports.begin();
        while(it != m_audio_ports.end() || *it != port) it++;
        if(it != m_audio_ports.end())
        {
            delete *it;
            m_audio_ports.erase(it);
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