//To be included in SoundDriver.cpp

#include <jack/jack.h>
#include <jack/midiport.h>
#include "CircularBuffer.hpp"
#include "LinkedList.hpp"

#define R64FX_PORT_IS_AUDIO_INPUT   0
#define R64FX_PORT_IS_AUDIO_OUTPUT  1
#define R64FX_PORT_IS_MIDI_INPUT    2
#define R64FX_PORT_IS_MIDI_OUTPUT   3

#define R64FX_JACK_SYNC_PORT_ENABLED     1

namespace r64fx{

class Jack;
class JackImpl;
class JackIOPort;
class JackIOPortHandle;
class JackIOPortImpl;
class JackIOPortImplHandle;
class JackSyncPort;
class JackSyncPortHande;


struct JackSyncPortImpl : public LinkedList<JackSyncPortImpl>::Node{
    JackSyncPortHande*                       iface       = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  to_iface    = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  from_iface  = nullptr;

    JackSyncPortImpl(int buffer_size)
    {
        to_iface   = new(std::nothrow) CircularBuffer<SoundDriverSyncMessage>(4);
        from_iface = new(std::nothrow) CircularBuffer<SoundDriverSyncMessage>(4);
    }

    ~JackSyncPortImpl()
    {
        delete to_iface;
        delete from_iface;
    }
};


class JackSyncPort : public SoundDriverSyncPort, public LinkedList<JackSyncPort>::Node{
    JackSyncPortImpl*                        m_impl       = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  m_to_impl    = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  m_from_impl  = nullptr;

public:
    JackSyncPort(JackSyncPortImpl* impl)
    : m_impl(impl)
    , m_to_impl(impl->from_iface)
    , m_from_impl(impl->to_iface)
    {
        
    }

    virtual void enable()
    {
        SoundDriverSyncMessage msg(1);
        m_to_impl->write(&msg, 1);
    }

    virtual void disable()
    {
        SoundDriverSyncMessage msg(0);
        m_to_impl->write(&msg, 1);
    }

    virtual int readMessages(SoundDriverSyncMessage* msgs, int nmsgs)
    {
        return m_from_impl->read(msgs, nmsgs);
    }
};


class JackMessage{
    unsigned long m_key    = 0;
    void* m_value  = 0;

public:
    JackMessage(){}

    JackMessage(unsigned long key, void* value)
    : m_key(key)
    , m_value(value)
    {}

    inline unsigned long key() const { return m_key; }

    inline void* value() const { return m_value; }
};


enum{
    AddIOPort,
    RemoveIOPort,
    AddSyncPort,
    RemoveSyncPort
};

enum{
    IOPortAdded,
    IOPortRemoved,
    SyncPortAdded,
    SyncPortRemoved
};


class JackImpl{
    LinkedList<JackIOPortImpl>    m_io_ports;
    LinkedList<JackSyncPortImpl>  m_sync_ports;
    CircularBuffer<JackMessage>*  m_from_iface   = nullptr;
    CircularBuffer<JackMessage>*  m_to_iface     = nullptr;

public:
    JackImpl(
        CircularBuffer<JackMessage>* to_impl,
        CircularBuffer<JackMessage>* from_impl
    )
    : m_from_iface(to_impl)
    , m_to_iface(from_impl)
    {
        
    }

    inline void msgAddIOPort(JackIOPortImpl* port_impl)
    {
        m_io_ports.append(port_impl);
        JackMessage msg(IOPortAdded, port_impl->iface);
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    inline void msgRemoveIOPort(JackIOPortImpl* port_impl)
    {
        m_io_ports.remove(port_impl);
        JackMessage msg(IOPortRemoved, port_impl->iface);
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    inline void msgAddSyncPort(JackSyncPortImpl* port_impl)
    {
        m_sync_ports.append(port_impl);
        JackMessage msg(SyncPortAdded, port_impl->iface);
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    inline void msgRemoveSyncPort(JackSyncPortImpl* port_impl)
    {
        m_sync_ports.remove(port_impl);
        JackMessage msg(SyncPortRemoved, port_impl->iface);
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    int process(int nframes)
    {
        JackMessage msg;
        while(m_from_iface->read(&msg, 1))
        {
            switch(msg.key())
            {
                case AddIOPort:
                {
                    msgAddIOPort((JackIOPortImpl*) msg.value());
                    break;
                }

                case RemoveIOPort:
                {
                    msgRemoveIOPort((JackIOPortImpl*) msg.value());
                    break;
                }

                case AddSyncPort:
                {
                    msgAddSyncPort((JackSyncPortImpl*) msg.value());
                    break;
                }

                case RemoveSyncPort:
                {
                    msgRemoveSyncPort((JackSyncPortImpl*) msg.value());
                    break;
                }
            }
        }

        for(auto port : m_io_ports)
        {
            void* port_buffer = jack_port_get_buffer(port->jack_port, nframes);

            unsigned long option = port->flags & 3;
            switch(option)
            {
                case R64FX_PORT_IS_AUDIO_INPUT:
                {
                    auto audio_in_port = (JackAudioPortImpl*)(port);
                    int nsamples = audio_in_port->buffer->write((float*)port_buffer, nframes);
                    (void)nsamples;
                    break;
                }

                case R64FX_PORT_IS_AUDIO_OUTPUT:
                {
                    auto audio_out_port = (JackAudioPortImpl*)(port);
                    int nsamples = audio_out_port->buffer->read((float*)port_buffer, nframes);
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

                        auto midi_in_port = (JackMidiPortImpl*)(port);
                        midi_in_port->buffer->write(MidiEvent(
                            MidiMessage(event.buffer, event.size), event.time
                        ));
                    }
                    break;
                }

                case R64FX_PORT_IS_MIDI_OUTPUT:
                {
                    jack_midi_clear_buffer(port_buffer);
                    
                    auto midi_out_port = (JackMidiPortImpl*)(port);
                    
                    MidiEvent midi_event;
                    while(midi_out_port->buffer->read(&midi_event, 1))
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
                    break;
                }

                default:
                    break;
            }
        }
        return 0;
    }
};


class Jack : public SoundDriver{
    jack_client_t*                           m_jack_client  = nullptr;
    JackImpl*                                m_impl         = nullptr;
    CircularBuffer<JackMessage>*  m_to_impl      = nullptr;
    CircularBuffer<JackMessage>*  m_from_impl    = nullptr;
    LinkedList<JackIOPort>                   m_io_ports;
    LinkedList<JackSyncPort>                 m_sync_ports;

public:
    Jack()
    {
        m_jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
        if(!m_jack_client)
            return;

        jack_set_error_function([](const char* message){
            cout << "Jack Error: " << message << "\n";
        });

        if(jack_set_process_callback(m_jack_client, [](jack_nframes_t nframes, void* arg) -> int {
            auto self = (JackImpl*) arg;
            return self->process(nframes);
        }, this) != 0)
        {
            jack_client_close(m_jack_client);
            return;
        }

        m_to_impl    = new(std::nothrow) CircularBuffer<JackMessage>(8);
        m_from_impl  = new(std::nothrow) CircularBuffer<JackMessage>(8);
        m_impl       = new(std::nothrow) JackImpl(m_to_impl, m_from_impl);
    }

    virtual ~Jack()
    {
        if(m_jack_client)
        {
            jack_client_close(m_jack_client);
            m_jack_client = nullptr;
        }
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

    template<typename PortT, typename PortImplT> PortT* newPort(const std::string &name, int buffer_size)
    {
        auto port_impl  = new(std::nothrow) PortImplT(buffer_size);
        auto port_iface = new(std::nothrow) PortT(this, port_impl);

        port_impl->jack_port = port_iface->jack_port = jack_port_register(
            m_jack_client,
            name.c_str(),
            (port_iface->type() == SoundDriverIOPort::Type::Audio) ?  JACK_DEFAULT_AUDIO_TYPE : JACK_DEFAULT_MIDI_TYPE,
            (port_iface->direction() == SoundDriverIOPort::Direction::Input) ?  JackPortIsInput : JackPortIsOutput,
            0
        );

        JackMessage msg(AddIOPort, port_impl);
        m_to_impl->write(&msg, 1);

        m_io_ports.append(port_iface);

        return port_iface;
    }

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name)
    {
        return newPort<JackAudioInput, JackAudioPortImpl>(name, bufferSize() * 2);
    }

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name)
    {
        return newPort<JackAudioOutput, JackAudioPortImpl>(name, bufferSize() * 2);
    }

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name)
    {
        return newPort<JackMidiInput, JackMidiPortImpl>(name, 32);
    }

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name)
    {
        return newPort<JackMidiOutput, JackMidiPortImpl>(name, 32);
    }

    virtual SoundDriverIOPort* findPort(const std::string &name)
    {
//         for(auto port : m_io_ports)
//         {
//             std::string port_name;
//             getPortName(port->iface, port_name);
//             if(port_name == name)
//                 return port->iface;
//         }
        return nullptr;
    }

    virtual void deletePort(SoundDriverIOPort* port)
    {
        auto io_port = dynamic_cast<JackIOPort*>(port);
        if(io_port)
        {
            JackMessage msg(RemoveIOPort, io_port->impl());
            m_to_impl->write(&msg, 1);
        }
    }

    virtual SoundDriverSyncPort* newSyncPort()
    {
        auto port_impl  = new(std::nothrow) JackSyncPortImpl(32);
        auto port_iface = new(std::nothrow) JackSyncPort(port_impl);

        JackMessage msg(AddSyncPort, port_impl);
        m_to_impl->write(&msg, 1);

        m_sync_ports.append(port_iface);

        return port_iface;
    }

    virtual void deleteSyncPort(SoundDriverSyncPort* port)
    {
        
    }

    virtual void setPortName(SoundDriverIOPort* port, const std::string &name)
    {
        auto port_iface = dynamic_cast<JackIOPort*>(port);
        jack_port_rename(m_jack_client, port_iface->jackPort(), name.c_str());
    }

    virtual void getPortName(SoundDriverIOPort* port, std::string &name)
    {
        auto port_iface = dynamic_cast<JackIOPort*>(port);
        name = std::string(jack_port_short_name(port_iface->jackPort()));
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
