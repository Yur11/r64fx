//To be included in SoundDriver.cpp

#include <jack/jack.h>
#include <jack/midiport.h>

namespace r64fx{

R64FX_SOUND_DRIVER_PORT_CLASSES(Jack, jack_port_t*)

class JackThreadImplHandle;

class JackThreadImpl : public SoundDriverThreadImpl<jack_port_t*>{
public:
    using SoundDriverThreadImpl<jack_port_t*>::SoundDriverThreadImpl;

    inline int process(int nframes)
    {
        prologue();

        for(auto port : ports())
        {
            void* port_buffer = jack_port_get_buffer(port->handle(), nframes);

            unsigned long option = port->flags() & R64FX_PORT_OPTION_MASK;
            switch(option)
            {
                case R64FX_PORT_IS_AUDIO_INPUT:
                {
                    auto audio_in_port = (InputPortImpl<float, jack_port_t*>*)(port);
                    int nsamples = audio_in_port->write((float*)port_buffer, nframes);
                    (void)nsamples;
                    break;
                }

                case R64FX_PORT_IS_AUDIO_OUTPUT:
                {
                    auto audio_out_port = (OutputPortImpl<float, jack_port_t*>*)(port);
                    int nsamples = audio_out_port->read((float*)port_buffer, nframes);
                    (void)nsamples;
                    break;
                }

                case R64FX_PORT_IS_MIDI_INPUT:
                {
                    int nevents = jack_midi_get_event_count(port_buffer);
                    for(int i=0; i<nevents; i++)
                    {
                        jack_midi_event_t event;
                        jack_midi_event_get(&event, port_buffer, i);

                        auto midi_in_port = (InputPortImpl<MidiEvent, jack_port_t*>*)(port);
                        MidiEvent midi_event(MidiMessage(event.buffer, event.size), event.time);
                        midi_in_port->write(&midi_event, 1);
                    }
                    break;
                }

                case R64FX_PORT_IS_MIDI_OUTPUT:
                {
                    jack_midi_clear_buffer(port_buffer);
                    
                    auto midi_out_port = (OutputPortImpl<MidiEvent, jack_port_t*>*)(port);
                    
                    MidiEvent midi_event;
                    while(midi_out_port->read(&midi_event, 1))
                    {
                        if(midi_event.message().byteCount() > 0)
                        {
                            jack_midi_event_write(
                                port_buffer, 
                                midi_event.time(), 
                                midi_event.message().bytes(), 
                                midi_event.message().byteCount()
                            );
                        }
                    }
                    break;
                }

                default:
                    break;
            }
        }
        
        epilogue();
        return 0;
    }
};


class Jack : public SoundDriverPartial<jack_port_t*>{
    jack_client_t*         m_jack_client  = nullptr;
    JackThreadImplHandle*  m_impl         = nullptr;

public:
    Jack()
    {
        m_jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        jack_set_error_function([](const char* message){
            std::cout << "Jack Error: " << message << "\n";
        });

        m_impl = (JackThreadImplHandle*) new(std::nothrow) JackThreadImpl(toImpl(), fromImpl());
        if(!m_impl)
        {
            jack_client_close(m_jack_client);
            return;
        }

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg) -> int {
            auto self = (JackThreadImpl*) arg;
            return self->process(nframes);
        }, m_impl) != 0)
        {
            jack_client_close(m_jack_client);
            return;
        }
    }

    virtual ~Jack()
    {
        if(m_jack_client)
            jack_client_close(m_jack_client);

        if(m_impl)
            delete (JackThreadImpl*)m_impl;
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

    template<typename PortT> PortT* newPort(const std::string &name, int buffer_size)
    {
        auto jack_port = jack_port_register(
            m_jack_client, name.c_str(),
            PortT::typeTraits().type() == SoundDriverPort::Type::Audio ? JACK_DEFAULT_AUDIO_TYPE : JACK_DEFAULT_MIDI_TYPE,
            PortT::portDirectionFlag() == R64FX_PORT_IS_AUDIO_INPUT ?  JackPortIsInput : JackPortIsOutput,
            0
        );
        if(!jack_port)
        {
            return nullptr;
        }

        auto port = new(std::nothrow) PortT(jack_port, buffer_size);
        if(!port)
        {
            jack_port_unregister(m_jack_client, jack_port);
            return nullptr;
        }

        if(!addPort(port))
        {
            jack_port_unregister(m_jack_client, jack_port);
            delete port;
            return nullptr;
        }

        return port;
    }

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name)
    {
        return newPort<JackAudioInput>(name, bufferSize() * 2);
    }

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name)
    {
        return newPort<JackAudioOutput>(name, bufferSize() * 2);
    }

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name)
    {
        return newPort<JackMidiInput>(name, 32);
    }

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name)
    {
        return newPort<JackMidiOutput>(name, 32);
    }

    virtual void deletePort(SoundDriverPort* port)
    {
        auto p = dynamic_cast<BasePortIface<jack_port_t*>*>(port);
        if(p)
        {
            removePort(p);
        }
    }

    virtual void portRemoved(BasePortIface<jack_port_t*>* port)
    {
        jack_port_unregister(m_jack_client, port->handle());
        delete port;
    }

    virtual void setPortName(SoundDriverPort* port, const std::string &name)
    {
        auto p = dynamic_cast<BasePortIface<jack_port_t*>*>(port);
        if(p)
        {
            jack_port_rename(m_jack_client, p->handle(), name.c_str());
        }
    }

    virtual void getPortName(SoundDriverPort* port, std::string &name)
    {
        auto p = dynamic_cast<BasePortIface<jack_port_t*>*>(port);
        if(p)
        {
            name = std::string(jack_port_short_name(p->handle()));
        }
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
