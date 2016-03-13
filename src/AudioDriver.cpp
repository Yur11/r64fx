#include "AudioDriver.hpp"
#include <jack/jack.h>
#include <new>
#include <iostream>

using namespace std;

namespace r64fx{

struct AudioDriver_Jack : public AudioDriver{
    jack_client_t* m_jack_client = nullptr;

    AudioDriver_Jack()
    {
        m_jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg)
        {
            auto self = (AudioDriver_Jack*) arg;
            static int num = 0;
            cout << "process: " << num++ << "\n";
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