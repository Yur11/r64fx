#include "SoundDriver.hpp"
#include <new>
#include <iostream>

using namespace std;

#define R64FX_PORT_GUTS(TYPE, DIRECTION)\
virtual Type type()\
{\
    return SoundDriverIOPort::Type::TYPE;\
}\
\
virtual Direction direction()\
{\
    return SoundDriverIOPort::Direction::DIRECTION;\
}\


struct JackIOPortImpl : public LinkedList<JackIOPortImpl>::Node {
    jack_port_t*       jack_port  = nullptr;
    JackIOPortHandle*  iface      = nullptr;
    unsigned long      flags      = 0;
};

struct JackAudioPortImpl : public JackIOPortImpl{
    CircularBuffer<float>* buffer = nullptr;

    JackAudioPortImpl(int buffer_size)
    {
        buffer = new(std::nothrow) CircularBuffer<float>(buffer_size);
    }

    ~JackAudioPortImpl()
    {
        delete buffer;
    }
};

struct JackMidiPortImpl : public JackIOPortImpl{
    CircularBuffer<MidiEvent>* buffer = nullptr;

    JackMidiPortImpl(int buffer_size)
    {
        buffer = new(std::nothrow) CircularBuffer<MidiEvent>(buffer_size);
    }

    ~JackMidiPortImpl()
    {
        delete buffer;
    }
};



class JackIOPort : public LinkedList<JackIOPort>::Node{
    Jack*                  m_parent_driver  = nullptr;
    JackIOPortImplHandle*  m_impl           = nullptr;
    jack_port_t*           m_jack_port      = nullptr;

public:
    JackIOPort(Jack* driver, JackIOPortImpl* impl)
    : m_parent_driver(driver)
    , m_impl((JackIOPortImplHandle*)impl)
    {
        impl->iface = (JackIOPortHandle*)this;
    }

    JackIOPortImplHandle* impl() const { return m_impl; }

    inline jack_port_t* jackPort() const { return m_jack_port; }

    Jack* parentDriver() const { return m_parent_driver; }
};

class JackAudioInput : public SoundDriverAudioInput, public JackIOPort{
    CircularBuffer<float>*  m_buffer   = nullptr;

public:
    JackAudioInput(Jack* driver, JackAudioPortImpl* impl) : JackIOPort(driver, impl)
    {
        impl->flags = (impl->flags & (~3UL)) | R64FX_PORT_IS_AUDIO_INPUT;
        m_buffer = impl->buffer;
    }

    virtual int readSamples(float* samples, int nsamples)
    {
        return m_buffer->read(samples, nsamples);
    }

    R64FX_PORT_GUTS(Audio, Input)
};

class JackAudioOutput : public SoundDriverAudioOutput, public JackIOPort{
    CircularBuffer<float>*  m_buffer  = nullptr;

public:
    JackAudioOutput(Jack* driver, JackAudioPortImpl* impl) : JackIOPort(driver, impl)
    {
        impl->flags = (impl->flags & (~3UL)) | R64FX_PORT_IS_AUDIO_OUTPUT;
        m_buffer = impl->buffer;
    }

    virtual int writeSamples(float* samples, int nsamples)
    {
        return m_buffer->write(samples, nsamples);
    }

    R64FX_PORT_GUTS(Audio, Output)
};

class JackMidiInput : public SoundDriverMidiInput, public JackIOPort{
    CircularBuffer<MidiEvent>*  m_buffer  = nullptr;

public:
    JackMidiInput(Jack* driver, JackMidiPortImpl* impl) : JackIOPort(driver, impl)
    {
        impl->flags = (impl->flags & (~3UL)) | R64FX_PORT_IS_MIDI_INPUT;
        m_buffer = impl->buffer;
    }

    virtual int readEvents(MidiEvent* events, int nevents)
    {
        return m_buffer->read(events, nevents);
    }

    R64FX_PORT_GUTS(Midi, Input)
};


class JackMidiOutput : public SoundDriverMidiOutput, public JackIOPort{
    CircularBuffer<MidiEvent>*  m_buffer  = nullptr;

public:
    JackMidiOutput(Jack* driver, JackMidiPortImpl* impl)  : JackIOPort(driver, impl)
    {
        impl->flags = (impl->flags & (~3UL)) | R64FX_PORT_IS_MIDI_OUTPUT;
        m_buffer = impl->buffer;
    }

    virtual int writeEvents(MidiEvent* events, int nevents)
    {
        return m_buffer->write(events, nevents);
    }

    R64FX_PORT_GUTS(Midi, Output)
};


#include "SoundDriver_Stub.cxx"
#ifdef R64FX_USE_JACK
#include "SoundDriver_Jack.cxx"
#endif//R64FX_USE_JACK


namespace r64fx{

SoundDriver* SoundDriver::newInstance(SoundDriver::Type type)
{
    switch(type)
    {
//         case SoundDriver::Type::Stub:
//         {
//             cout << "stub!\n";
//             driver = new(std::nothrow) SoundDriver_Stub;
//             break;
//         }

        case SoundDriver::Type::Jack:
        {
            return new(std::nothrow) Jack;
        }

        default:
        {
            return nullptr;
            break;
        }
    }
}


void SoundDriver::deleteInstance(SoundDriver* driver)
{
    delete driver;
}

}//namespace r64fx
