//To be included in SoundDriver.cpp

#include <jack/jack.h>
#include <jack/midiport.h>
#include "CircularBuffer.hpp"
#include "LinkedList.hpp"
#include "TimeUtils.hpp"

namespace r64fx{

struct SoundDriverIOPort_Jack
: public LinkedList<SoundDriverIOPort_Jack>::Node {
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
    SoundDriverIOPort_Jack* impl = nullptr;
    CircularBuffer<float> buffer;

    SoundDriverIOPort_AudioInput_Jack(int buffer_size)
    : buffer(buffer_size)
    {

    }

    virtual ~SoundDriverIOPort_AudioInput_Jack() {}

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

    virtual int readSamples(float* samples, int nsamples)
    {
        return buffer.read(samples, nsamples);
    }
};


struct SoundDriverIOPort_AudioOutput_Jack : public SoundDriverIOPort_AudioOutput{
    SoundDriverIOPort_Jack* impl = nullptr;
    CircularBuffer<float> buffer;

    SoundDriverIOPort_AudioOutput_Jack(int buffer_size)
    : buffer(buffer_size)
    {

    }

    virtual ~SoundDriverIOPort_AudioOutput_Jack()
    {

    }

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

    virtual int writeSamples(float* samples, int nsamples)
    {
        return buffer.write(samples, nsamples);
    }
};


struct SoundDriverIOPort_MidiInput_Jack : public SoundDriverIOPort_MidiInput{
    SoundDriverIOPort_Jack* impl = nullptr;
    CircularBuffer<MidiEvent> buffer;

    SoundDriverIOPort_MidiInput_Jack(int buffer_size)
    : buffer(buffer_size)
    {

    }

    virtual ~SoundDriverIOPort_MidiInput_Jack() {}

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

    virtual int readEvents(MidiEvent* events, int nevents)
    {
        return buffer.read(events, nevents);
    }
};


struct SoundDriverIOPort_MidiOutput_Jack : public SoundDriverIOPort_MidiOutput{
    SoundDriverIOPort_Jack* impl = nullptr;
    CircularBuffer<MidiEvent> buffer;

    SoundDriverIOPort_MidiOutput_Jack(int buffer_size)
    : buffer(buffer_size)
    {

    }

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
    
    virtual int writeEvents(MidiEvent* events, int nevents)
    {
        return buffer.write(events, nevents);
    }
};


struct SoundDriverIOStatusPort_Jack
: public SoundDriverIOStatusPort
, public LinkedList<SoundDriverIOStatusPort_Jack>::Node{
    CircularBuffer<SoundDriverIOStatus> buffer;

    SoundDriverIOStatusPort_Jack()
    : buffer(2)
    {

    }

    virtual int readStatus(SoundDriverIOStatus* status, int nitems = 1)
    {
        return buffer.read(status, nitems);
    }
};


struct SoundDriver_Jack : public SoundDriver{
    jack_client_t* m_jack_client = nullptr;

    LinkedList<SoundDriverIOPort_Jack> m_ports;
    CircularBuffer<SoundDriverIOPort_Jack*> m_new_ports;
    CircularBuffer<SoundDriverIOPort_Jack*> m_deleted_ports;
    int m_port_count = 0;

    LinkedList<SoundDriverIOStatusPort_Jack> m_status_ports;
    CircularBuffer<SoundDriverIOStatusPort_Jack*> m_new_status_ports;

    SoundDriver_Jack()
    : m_new_ports(16)
    , m_deleted_ports(16)
    , m_new_status_ports(2)
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
        SoundDriverIOStatus status;
        status.begin_time = current_nanoseconds();

        SoundDriverIOPort_Jack* new_port = nullptr;
        while(m_new_ports.read(&new_port, 1))
        {
            if(new_port)
            {
                m_ports.append(new_port);
            }
            new_port = nullptr;
        }

        
        SoundDriverIOPort_Jack* deleted_port = nullptr;
        while(m_deleted_ports.read(&deleted_port, 1))
        {
            if(deleted_port)
            {
                m_ports.remove(deleted_port);
            }
            deleted_port = nullptr;
        }
        

        for(auto port : m_ports)
        {
            void* port_buffer = jack_port_get_buffer(port->jack_port, nframes);

            if(port->is_input)
            {
                if(port->is_audio)
                {
                    auto audio_in_port = (SoundDriverIOPort_AudioInput_Jack*)(port->iface);
                    int nsamples = audio_in_port->buffer.write((float*)port_buffer, nframes);
                    (void)nsamples;
                }
                else
                {
                    int nevents = jack_midi_get_event_count(port_buffer);
                    for(int i=0; i<nevents; i++)
                    {
                        jack_midi_event_t event;
                        jack_midi_event_get(&event, port_buffer, i);

                        auto midi_in_port = (SoundDriverIOPort_MidiInput_Jack*)(port->iface);
                        midi_in_port->buffer.write(MidiEvent(
                            MidiMessage(event.buffer, event.size), event.time
                        ));
                    }
                }
            }
            else
            {
                if(port->is_audio)
                {
                    auto audio_out_port = (SoundDriverIOPort_AudioOutput_Jack*)(port->iface);
                    int nsamples = audio_out_port->buffer.read((float*)port_buffer, nframes);
                    (void)nsamples;
                }
                else
                {
                    jack_midi_clear_buffer(port_buffer);
                    
                    auto midi_out_port = (SoundDriverIOPort_MidiOutput_Jack*)(port->iface);
                    
                    MidiEvent midi_event;
                    while(midi_out_port->buffer.read(&midi_event, 1))
                    {
                        if(midi_event.message().byteCount() > 0)
                        {
                            cout << jack_midi_event_write(
                                port_buffer, 
                                midi_event.time(), 
                                midi_event.message().bytes(), 
                                midi_event.message().byteCount()
                            );
                        }
                    }
                }
            }
        }

        SoundDriverIOStatusPort_Jack* status_port = nullptr;
        while(m_new_status_ports.read(&status_port, 1))
        {
            if(status_port)
            {
                m_status_ports.append(status_port);
            }
            status_port = nullptr;
        }

        status.end_time = current_nanoseconds();
        for(auto status_port : m_status_ports)
        {
            status_port->buffer.write(&status, 1);
        }

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


    template<typename PortT> PortT* newPort(const std::string &name, int buffer_size)
    {
        auto port_iface = new(std::nothrow) PortT(buffer_size);
        if(!port_iface)
            return nullptr;

        bool is_audio = (port_iface->type()       ==  SoundDriverIOPort::Type::Audio);
        bool is_input = (port_iface->direction()  ==  SoundDriverIOPort::Direction::Input);

        auto port_impl = new(std::nothrow) SoundDriverIOPort_Jack;
        if(!port_impl)
        {
            delete port_iface;
            return nullptr;
        }

        port_impl->jack_port = jack_port_register(
            m_jack_client,
            name.c_str(),
            is_audio ?  JACK_DEFAULT_AUDIO_TYPE : JACK_DEFAULT_MIDI_TYPE,
            is_input ?  JackPortIsInput : JackPortIsOutput,
            0
        );
        if(!port_impl->jack_port)
        {
            delete port_iface;
            delete port_impl;
            return nullptr;
        }

        port_iface->impl = port_impl;
        port_impl->iface = port_iface;
        port_impl->is_input = is_input;
        port_impl->is_audio = is_audio;

        m_new_ports.write(port_impl);
        m_port_count++;
        
        return port_iface;
    }

    
    virtual SoundDriverIOPort_AudioInput* newAudioInput(const std::string &name)
    {
        return newPort<SoundDriverIOPort_AudioInput_Jack>(name, bufferSize() * 2);
    }


    virtual SoundDriverIOPort_AudioOutput* newAudioOutput(const std::string &name)
    {
        return newPort<SoundDriverIOPort_AudioOutput_Jack>(name, bufferSize() * 2);
    }


    virtual SoundDriverIOPort_MidiInput* newMidiInput(const std::string &name)
    {
        return newPort<SoundDriverIOPort_MidiInput_Jack>(name, 32);
    }
    
    
    virtual SoundDriverIOPort_MidiOutput* newMidiOutput(const std::string &name)
    {
        return newPort<SoundDriverIOPort_MidiOutput_Jack>(name, 32);
    }

    
    virtual SoundDriverIOPort* findPort(const std::string &name)
    {
        for(auto port : m_ports)
        {
            if(port->name() == name)
                return port->iface;
        }
        return nullptr;
    }
    

    virtual void deletePort(SoundDriverIOPort* port)
    {
        auto jack_port = dynamic_cast<SoundDriverIOPort_Jack*>(port);
        if(jack_port)
        {
            m_deleted_ports.write(&jack_port, 1);
        }
    }

    
    virtual SoundDriverIOStatusPort* newStatusPort()
    {
        auto port = new(std::nothrow) SoundDriverIOStatusPort_Jack;
        if(!port)
            return nullptr;

        m_new_status_ports.write(port);
        return port;
    }
    
    virtual void deletePort(SoundDriverIOStatusPort* port)
    {
        
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
