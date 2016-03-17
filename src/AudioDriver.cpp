#include "AudioDriver.hpp"
#include "LinkedList.hpp"
#include <jack/jack.h>
#include <jack/midiport.h>
#include <cstring>
#include <new>
#include <vector>
#include <iostream>

#define m_private ((AudioDriverIOPortPrivate*)m)

using namespace std;

namespace r64fx{


struct AudioDriverIOPortPrivate{
    union PortIface{

    } iface;

    AudioDriverIOPort::Type type = AudioDriverIOPort::Type::Bad;

    string name = "";
};


AudioDriverIOPort::Type AudioDriverIOPort::type() const
{
    return m_private->type;
}


void AudioDriverIOPort::setName(const std::string &name)
{
    m_private->name = name;
}


std::string AudioDriverIOPort::name() const
{
    return m_private->name;
}


float* AudioDriverIOPort_Audio::buffer() const
{
    return 0;
}


MidiEvent* AudioDriverIOPort_Midi::events() const
{
    return nullptr;
}


int AudioDriverIOPort_Midi::eventCount()
{
    return 0;
}


struct AudioDriver_Jack : public AudioDriver{
    jack_client_t* m_jack_client = nullptr;
    LinkedList<AudioDriverIOPortPrivate> m_ports;

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
        return 0;
    }


    virtual ~AudioDriver_Jack()
    {
        if(m_jack_client)
        {
            jack_client_close(m_jack_client);
            m_jack_client = nullptr;
        }
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