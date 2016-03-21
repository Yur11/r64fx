//To be included in SoundDriver.cpp

#include <jack/jack.h>
#include <jack/midiport.h>

namespace r64fx{

struct SoundDriverIOPort_ImplJack{
    SoundDriverIOPort*  iface      = nullptr;
    jack_port_t*        jack_port  = nullptr;

    bool is_audio = false;
    bool is_input = false;

    void setName(const std::string &name)
    {
        jack_port_set_name(jack_port, name.c_str());
    }

    const char* name() const
    {
        if(!jack_port)
            return "";
        else
            return jack_port_short_name(jack_port);
    }
};


struct SoundDriverIOPort_AudioInput_Jack : public SoundDriverIOPort_AudioInput{
    SoundDriverIOPort_ImplJack* impl = nullptr;

    virtual Type type()
    {
        return SoundDriverIOPort::Type::Audio;
    }

    virtual Direction direction()
    {
        return SoundDriverIOPort::Direction::Input;
    }

    virtual void setName(const std::string &name)
    {
        impl->setName(name);
    }

    virtual std::string name()
    {
        return impl->name();
    }
};


struct SoundDriverIOPort_AudioOutput_Jack : public SoundDriverIOPort_AudioOutput{
    SoundDriverIOPort_ImplJack* impl = nullptr;

    virtual Type type()
    {
        return SoundDriverIOPort::Type::Audio;
    }

    virtual Direction direction()
    {
        return SoundDriverIOPort::Direction::Output;
    }

    virtual void setName(const std::string &name)
    {
        impl->setName(name);
    }

    virtual std::string name()
    {
        return impl->name();
    }
};


struct SoundDriverIOPort_MidiInput_Jack : public SoundDriverIOPort_MidiInput{
    SoundDriverIOPort_ImplJack* impl = nullptr;

    virtual Type type()
    {
        return SoundDriverIOPort::Type::Midi;
    }

    virtual Direction direction()
    {
        return SoundDriverIOPort::Direction::Input;
    }

    virtual void setName(const std::string &name)
    {
        impl->setName(name);
    }

    virtual std::string name()
    {
        return impl->name();
    }
};


struct SoundDriverIOPort_MidiOutput_Jack : public SoundDriverIOPort_MidiOutput{
    SoundDriverIOPort_ImplJack* impl = nullptr;

    virtual Type type()
    {
        return SoundDriverIOPort::Type::Midi;
    }

    virtual Direction direction()
    {
        return SoundDriverIOPort::Direction::Output;
    }

    virtual void setName(const std::string &name)
    {
        impl->setName(name);
    }

    virtual std::string name()
    {
        return impl->name();
    }
};


struct SoundDriver_Jack : public SoundDriver{
    jack_client_t* m_jack_client = nullptr;

    volatile long m_count = 0;

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


    virtual SoundDriver::Type type()
    {
        return SoundDriver::Type::Jack;
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

    virtual SoundDriverIOPort_AudioOutput* newAudioOutput(const std::string &name = "")
    {
        auto port = new SoundDriverIOPort_AudioOutput_Jack;
        return port;
    }

};

}//namespace r64fx