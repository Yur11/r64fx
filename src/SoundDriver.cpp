#include "SoundDriver.hpp"
#include "LinkedList.hpp"
#include <jack/jack.h>
#include <jack/midiport.h>
#include <cstring>
#include <new>
#include <vector>
#include <iostream>

#define m_private ((SoundDriverIOPortPrivate*)m)

using namespace std;

namespace r64fx{


struct SoundDriverIOPortPrivate{
    union PortIface{

    } iface;

    SoundDriverIOPort::Type type = SoundDriverIOPort::Type::Bad;

    string name = "";
};


SoundDriverIOPort::Type SoundDriverIOPort::type() const
{
    return m_private->type;
}


void SoundDriverIOPort::setName(const std::string &name)
{
    m_private->name = name;
}


std::string SoundDriverIOPort::name() const
{
    return m_private->name;
}


float* SoundDriverIOPort_Audio::buffer() const
{
    return 0;
}


MidiEvent* SoundDriverIOPort_Midi::events() const
{
    return nullptr;
}


int SoundDriverIOPort_Midi::eventCount()
{
    return 0;
}


struct SoundDriver_Jack : public SoundDriver{
    jack_client_t* m_jack_client = nullptr;
    volatile long m_count = 0;
    LinkedList<SoundDriverIOPortPrivate> m_ports;

    SoundDriver_Jack()
    {
        m_jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        jack_set_error_function([](const char* message){
            cout << "Jack Error: " << message << "\n";
        });

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg) -> int {
            auto self = (SoundDriver_Jack*) arg;
            return self->process(nframes);
        }, this) != 0)
        {
            jack_client_close(m_jack_client);
        }
    }


    int process(int nframes)
    {
        m_count++;
        return 0;
    }


    virtual ~SoundDriver_Jack()
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

    virtual long count()
    {
        return m_count;
    }
};


SoundDriver* SoundDriver::newInstance(SoundDriver::Type type)
{
    SoundDriver* driver = nullptr;
    switch(type)
    {
        case SoundDriver::Type::Jack:
        {
            driver = new(std::nothrow) SoundDriver_Jack;
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


void SoundDriver::deleteInstance(SoundDriver* driver)
{
    delete driver;
}

}//namespace r64fx