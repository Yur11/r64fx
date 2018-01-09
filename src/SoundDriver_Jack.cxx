//To be included in SoundDriver.cpp

#include <jack/jack.h>
#include <jack/midiport.h>

namespace r64fx{

namespace{

struct JackPortImpl : public SoundDriverPortImpl{
    jack_port_t* jack_port;
};


struct SoundDriverImplJack{
    inline static void* portBuffer(SoundDriverPortImpl* port, int nframes)
    {
        auto port_impl = (JackPortImpl*)port;
        return jack_port_get_buffer(port_impl->jack_port, nframes);
    }

    inline static void processAudioInput(SoundDriverPortImpl* port, int nframes, float* buffer)
    {
        memcpy(buffer, portBuffer(port, nframes), nframes * sizeof(float));
    }

    inline static void processAudioOutput(SoundDriverPortImpl* port, int nframes, float* buffer)
    {
        memcpy(portBuffer(port, nframes), buffer, nframes * sizeof(float));
    }

    inline static void processMidiInput(SoundDriverPortImpl* port, int nframes, MidiEventBuffer &buffer)
    {
        auto port_buffer = portBuffer(port, nframes);
        int nevents = jack_midi_get_event_count(port_buffer);
        for(int i=0; i<nevents; i++)
        {
            jack_midi_event_t event;
            jack_midi_event_get(&event, port_buffer, i);
            MidiMessage midimsg(event.buffer, event.size);
            buffer.write(midimsg, event.time);
        }
    }

    inline static void processMidiOutput(SoundDriverPortImpl* port, int nframes, MidiEventBuffer &buffer)
    {
        auto port_buffer = portBuffer(port, nframes);
        jack_midi_clear_buffer(port_buffer);

        MidiEvent event;
        while(buffer.read(event))
        {
            jack_midi_event_write(port_buffer, event.time, event.bytes(), event.byteCount());
        }
    }

    inline void exitThread() { /* Not used with jack! */ }
};

}//namespace

class SoundDriverImplJackHandle;


class SoundDriver_Jack : public SoundDriver{
    jack_client_t*              m_jack_client  = nullptr;
    SoundDriverImplJackHandle*  m_impl         = nullptr;

public:
    SoundDriver_Jack(const char* client_name)
    {
        m_jack_client = jack_client_open(client_name, JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        jack_set_error_function([](const char* message){
            std::cout << "Jack Error: " << message << "\n";
        });

        m_impl = (SoundDriverImplJackHandle*) new(std::nothrow) SoundDriverImpl<SoundDriverImplJack>(&m_to_impl, &m_from_impl);
        if(!m_impl)
        {
            jack_client_close(m_jack_client);
            return;
        }

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg) -> int {
            auto self = (SoundDriverImpl<SoundDriverImplJack>*) arg;
            self->process(nframes);
            return 0;
        }, m_impl) != 0)
        {
            jack_client_close(m_jack_client);
            return;
        }
    }

    virtual ~SoundDriver_Jack()
    {
        if(m_jack_client)
            jack_client_close(m_jack_client);

        if(m_impl)
            delete (SoundDriverImpl<SoundDriverImplJack>*)m_impl;
    }

private:
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

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name)
    {
        auto port_impl = new JackPortImpl;
        port_impl->bits |= R64FX_PORT_IS_AUDIO_INPUT;
        port_impl->jack_port = jack_port_register(m_jack_client, name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
        return (SoundDriverAudioInput*)port_impl;
    }

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name)
    {
        auto port_impl = new JackPortImpl;
        port_impl->bits |= R64FX_PORT_IS_AUDIO_OUTPUT;
        port_impl->jack_port = jack_port_register(m_jack_client, name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        return (SoundDriverAudioOutput*)port_impl;
    }

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name)
    {
        auto port_impl = new JackPortImpl;
        port_impl->bits |= R64FX_PORT_IS_MIDI_INPUT;
        port_impl->jack_port = jack_port_register(m_jack_client, name.c_str(), JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
        return (SoundDriverMidiInput*)port_impl;
    }

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name)
    {
        auto port_impl = new JackPortImpl;
        port_impl->bits |= R64FX_PORT_IS_MIDI_OUTPUT;
        port_impl->jack_port = jack_port_register(m_jack_client, name.c_str(), JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
        return (SoundDriverMidiOutput*)port_impl;
    }

    virtual void deletePort(SoundDriverPort* port)
    {
        auto port_impl = (JackPortImpl*)port;
        R64FX_DEBUG_ASSERT(port_impl->buffer == nullptr);
        R64FX_DEBUG_ASSERT(port_impl->jack_port);
        jack_port_unregister(m_jack_client, port_impl->jack_port);
        delete port_impl;
    }

    virtual void setPortName(SoundDriverPort* port, const std::string &name)
    {
        auto port_impl = (JackPortImpl*)port;
        R64FX_DEBUG_ASSERT(port_impl->jack_port);
        jack_port_rename(m_jack_client, port_impl->jack_port, name.c_str());
    }

    virtual void getPortName(SoundDriverPort* port, std::string &name)
    {
        auto port_impl = (JackPortImpl*)port;
        R64FX_DEBUG_ASSERT(port_impl->jack_port);
        name = std::string(jack_port_short_name(port_impl->jack_port));
    }

    virtual bool connect(const std::string &src, const std::string &dst)
    {
        return jack_connect(m_jack_client, src.c_str(), dst.c_str()) == 0;
    }

    virtual bool disconnect(const std::string &src, const std::string &dst)
    {
        return jack_disconnect(m_jack_client, src.c_str(), dst.c_str()) == 0;
    }
};

}//namespace r64fx
