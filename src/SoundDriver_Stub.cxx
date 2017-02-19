//To be included in SoundDriver.cpp

#include "CircularBuffer.hpp"
#include "LinkedList.hpp"
#include "Thread.hpp"
#include "TimeUtils.hpp"

#ifndef R64FX_STUB_BUFFER_SIZE
#define R64FX_STUB_BUFFER_SIZE 256
#endif//R64FX_STUB_BUFFER_SIZE

#ifndef R64FX_STUB_SAMPLE_RATE
#define R64FX_STUB_SAMPLE_RATE 48000
#endif//R64FX_STUB_SAMPLE_RATE

#define R64FX_STUB_SLEEP_NANOSECONDS ((float(R64FX_STUB_BUFFER_SIZE) / float(R64FX_STUB_SAMPLE_RATE)) * 1000 * 1000 * 1000)

namespace r64fx{

R64FX_SOUND_DRIVER_PORT_CLASSES(Stub, void*)

class StubThreadImplHandle;

class StubThreadImpl : public SoundDriverThreadImpl<void*>{
    float  m_silence  [R64FX_STUB_BUFFER_SIZE];
    float  m_stub     [R64FX_STUB_BUFFER_SIZE];
    CircularBuffer<long>* m_thread_terminator = nullptr;

public:
    StubThreadImpl(
        CircularBuffer<long>*                thread_terminator,
        CircularBuffer<SoundDriverMessage>*  to_impl,
        CircularBuffer<SoundDriverMessage>*  from_impl
    )
    : SoundDriverThreadImpl<void*>(to_impl, from_impl)
    , m_thread_terminator(thread_terminator)
    {
        for(int i=0; i<R64FX_STUB_BUFFER_SIZE; i++)
        {
            m_silence[i] = 0.0f;
        }
    }

    inline int process(int nframes)
    {
        prologue();

        for(auto port : ports())
        {
            unsigned long option = port->flags() & R64FX_PORT_OPTION_MASK;
            switch(option)
            {
                case R64FX_PORT_IS_AUDIO_INPUT:
                {
                    auto audio_in_port = (InputPortImpl<float, void*>*)(port);
                    for(int i=0; i<R64FX_STUB_BUFFER_SIZE; i++)
                    {
                        audio_in_port->write(m_silence, nframes);
                    }
                    break;
                }

                case R64FX_PORT_IS_AUDIO_OUTPUT:
                {
                    auto audio_out_port = (OutputPortImpl<float, void*>*)(port);
                    for(int i=0; i<R64FX_STUB_BUFFER_SIZE; i++)
                    {
                        audio_out_port->read(m_stub, nframes);
                    }
                    break;
                }

                case R64FX_PORT_IS_MIDI_INPUT:
                {
                    break;
                }

                case R64FX_PORT_IS_MIDI_OUTPUT:
                {
                    auto midi_out_port = (OutputPortImpl<MidiEvent, void*>*)(port);
                    MidiEvent midi_event[4];
                    while(midi_out_port->read(midi_event, 4)) {}
                    break;
                }

                default:
                    break;
            }
        }

        epilogue();
        return 0;
    }

    inline void runThread()
    {
        bool running = true;
        while(running)
        {
            long msg = 0;
            if(m_thread_terminator->read(&msg, 1))
                running = false;

            process(R64FX_STUB_BUFFER_SIZE);
            sleep_nanoseconds(R64FX_STUB_SLEEP_NANOSECONDS);
        }
    }
};


class Stub : public SoundDriverPartial<void*>{
    StubThreadImplHandle*  m_impl         = nullptr;
    Thread*                m_thread       = nullptr;
    CircularBuffer<long>   m_thread_terminator;

public:
    Stub()
    : m_thread_terminator(2)
    {
        m_impl = (StubThreadImplHandle*) new(std::nothrow) StubThreadImpl(&m_thread_terminator, toImpl(), fromImpl());
    }

    virtual ~Stub()
    {
        if(m_impl)
            delete (StubThreadImpl*)m_impl;
    }

private:
    virtual SoundDriver::Type type()
    {
        return SoundDriver::Type::Stub;
    }

    virtual bool isGood()
    {
        return m_impl != nullptr;
    }

    virtual void enable()
    {
#ifdef R64FX_DEBUG
        assert(m_impl != nullptr);
#endif//R64FX_DEBUG
        if(m_impl == nullptr)
            return;

        if(m_thread == nullptr)
        {
            m_thread = new(std::nothrow) Thread;
            m_thread->run([](void* arg) -> void*{
                auto impl = (StubThreadImpl*) arg;
                impl->runThread();
                return nullptr;
            }, m_impl);
        }
    }

    virtual void disable()
    {
        long msg = 1;
        m_thread_terminator.write(&msg, 1);
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }

    virtual int bufferSize()
    {
        return R64FX_STUB_BUFFER_SIZE;
    }

    virtual int sampleRate()
    {
        return R64FX_STUB_SAMPLE_RATE;
    }

    template<typename PortT> PortT* newPort(const std::string &name, int buffer_size)
    {
        auto port = new(std::nothrow) PortT(nullptr, buffer_size);
        if(!port)
            return nullptr;

        if(!addPort(port))
        {
            delete port;
            return nullptr;
        }

        return port;
    }

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name)
    {
        return newPort<StubAudioInput>(name, bufferSize());
    }

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name)
    {
        return newPort<StubAudioOutput>(name, bufferSize());
    }

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name)
    {
        return newPort<StubMidiInput>(name, 32);
    }

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name)
    {
        return newPort<StubMidiOutput>(name, 32);
    }

    virtual void deletePort(SoundDriverPort* port)
    {
        auto p = dynamic_cast<BasePortIface<void*>*>(port);
        if(p)
        {
            removePort(p);
        }
    }

    virtual void portRemoved(BasePortIface<void*>* port)
    {
        delete port;
    }

    virtual void setPortName(SoundDriverPort* port, const std::string &name)
    {
    }

    virtual void getPortName(SoundDriverPort* port, std::string &name)
    {
    }

    virtual bool connect(const std::string &src, const std::string &dst)
    {
        return false;
    }

    virtual bool disconnect(const std::string &src, const std::string &dst)
    {
        return false;
    }
};

}//namespace r64fx
