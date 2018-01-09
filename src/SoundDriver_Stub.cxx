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

namespace{

struct SoundDriverStubImpl{
    bool m_running = true;

    inline void processPort(SoundDriverPortImpl* port, int nframes)
    {
        unsigned long option = port->bits & R64FX_PORT_OPTION_MASK;
        switch(option)
        {
            case R64FX_PORT_IS_AUDIO_INPUT:
                break;

            case R64FX_PORT_IS_AUDIO_OUTPUT:
            {
                
                break;
            }

            case R64FX_PORT_IS_MIDI_INPUT:
                break;

            case R64FX_PORT_IS_MIDI_OUTPUT:
                break;

            default:
                break;
        }
    }

    inline static void processAudioInput(SoundDriverPortImpl* port, int nframes, float* buffer) {}

    inline static void processAudioOutput(SoundDriverPortImpl* port, int nframes, float* buffer)
    {
        auto buff = (float*) port->buffer;
        for(int i=0; i<nframes; i++)
            buff[i] = 0.0f;
    }

    inline static void processMidiInput(SoundDriverPortImpl* port, int nframes, MidiEventBuffer &buffer) {}

    inline static void processMidiOutput(SoundDriverPortImpl* port, int nframes, MidiEventBuffer &buffer) {}

    inline void exitThread() { m_running = false; }
};

struct SoundDriverStubThread : public SoundDriverImpl<SoundDriverStubImpl>{
    using SoundDriverImpl<SoundDriverStubImpl>::SoundDriverImpl;

    inline void runThread()
    {
        while(m_running)
        {
            process(R64FX_STUB_BUFFER_SIZE);
            sleep_nanoseconds(R64FX_STUB_SLEEP_NANOSECONDS);
        }
    }
};

}//namespace

class SoundDriverStubThreadHandle;


class SoundDriver_Stub : public SoundDriver{
    SoundDriverStubThreadHandle*  m_impl    = nullptr;
    Thread*                       m_thread  = nullptr;

public:
    SoundDriver_Stub()
    {
        m_impl = (SoundDriverStubThreadHandle*) new SoundDriverStubThread(&m_to_impl, &m_from_impl);
    }

    virtual ~SoundDriver_Stub()
    {
        if(m_impl)
            delete (SoundDriverStubThread*)m_impl;
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
                auto impl = (SoundDriverStubThread*) arg;
                impl->runThread();
                return nullptr;
            }, m_impl);
        }
    }

    virtual void disable()
    {
        unsigned long msg = R64FX_STUB_THREAD_EXIT;
        m_to_impl.write(&msg, 1);
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

    virtual SoundDriverAudioInput* newAudioInput(const std::string &name)
    {
        auto port = new SoundDriverPortImpl;
        port->bits |= R64FX_PORT_IS_AUDIO_INPUT;
        return (SoundDriverAudioInput*)port;
    }

    virtual SoundDriverAudioOutput* newAudioOutput(const std::string &name)
    {
        auto port = new SoundDriverPortImpl;
        port->bits |= R64FX_PORT_IS_AUDIO_OUTPUT;
        return (SoundDriverAudioOutput*)port;
    }

    virtual SoundDriverMidiInput* newMidiInput(const std::string &name)
    {
        auto port = new SoundDriverPortImpl;
        port->bits |= R64FX_PORT_IS_MIDI_INPUT;
        return (SoundDriverMidiInput*)port;
    }

    virtual SoundDriverMidiOutput* newMidiOutput(const std::string &name)
    {
        auto port = new SoundDriverPortImpl;
        port->bits |= R64FX_PORT_IS_MIDI_OUTPUT;
        return (SoundDriverMidiOutput*)port;
    }

    virtual void deletePort(SoundDriverPort* port)
    {
        auto port_impl = (SoundDriverPortImpl*)port;
        R64FX_DEBUG_ASSERT(port_impl->buffer == 0);
        delete port_impl;
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
