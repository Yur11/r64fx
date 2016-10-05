//To be included in SoundDriver.cpp

#include "CircularBuffer.hpp"
#include "LinkedList.hpp"
#include "Thread.hpp"
#include "current_time.hpp"
#include "sleep.hpp"

namespace r64fx{
    

class SoundDriverIOPort_AudioInput_Stub : public SoundDriverIOPort_AudioInput{
    std::string m_name = "";
    
public:
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
        m_name = name;
    }

    virtual std::string name()
    {
        return m_name;
    }
    
    virtual int readSamples(float* samples, int nsamples)
    {
        for(int i=0; i<nsamples; i++)
        {
            samples[i] = 0.0f;
        }
        return nsamples;
    }
};


class SoundDriverIOPort_AudioOutput_Stub : public SoundDriverIOPort_AudioOutput{
    std::string m_name = "";
    
public:
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
        m_name = name;
    }

    virtual std::string name()
    {
        return m_name;
    }

    virtual int writeSamples(float* samples, int nsamples)
    {
        return nsamples;
    }
};


class SoundDriverIOPort_MidiInput_Stub : public SoundDriverIOPort_MidiInput{
    std::string m_name = "";
    
public:
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
        m_name = name;
    }

    virtual std::string name()
    {
        return m_name;
    }
    
    virtual int readEvents(MidiEvent* events, int nevents)
    {
        return 0;
    }
};


class SoundDriverIOPort_MidiOutput_Stub : public SoundDriverIOPort_MidiOutput{
    std::string m_name = "";
    
public:
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
        m_name = name;
    }

    virtual std::string name()
    {
        return m_name;
    }
    
    virtual int writeEvents(MidiEvent* events, int nevents)
    {
        return nevents;
    }
};


struct SoundDriverIOStatusPort_Stub
: public SoundDriverIOStatusPort
, public LinkedList<SoundDriverIOStatusPort_Stub>::Node{
    CircularBuffer<SoundDriverIOStatus> buffer;

    SoundDriverIOStatusPort_Stub()
    : buffer(2)
    {

    }

    virtual int readStatus(SoundDriverIOStatus* status, int nitems = 1)
    {
        return buffer.read(status, nitems);
    }
};


class SoundDriver_Stub : public SoundDriver{
    Thread m_thread;
    LinkedList<SoundDriverIOStatusPort_Stub> m_status_ports;
    bool m_running = true;
    
public:
    virtual SoundDriver::Type type()
    {
        return SoundDriver::Type::Stub;
    }

    virtual void enable()
    {
        m_running = true;
        m_thread = Thread();
        m_thread.run([](void* arg) -> void* {
            auto self = (SoundDriver_Stub*) arg;
            return self->thread();
        }, this);
    }

    virtual void disable()
    {
        m_running = false;
        m_thread.join();
    }

    virtual int bufferSize()
    {
        return 256;
    }

    virtual int sampleRate()
    {
        return 48000;
    }

    virtual SoundDriverIOPort_AudioInput* newAudioInput(const std::string &name)
    {
        return new(std::nothrow) SoundDriverIOPort_AudioInput_Stub;
    }

    virtual SoundDriverIOPort_AudioOutput* newAudioOutput(const std::string &name)
    {
        return new(std::nothrow) SoundDriverIOPort_AudioOutput_Stub;
    }

    virtual SoundDriverIOPort_MidiInput* newMidiInput(const std::string &name)
    {
        return new(std::nothrow) SoundDriverIOPort_MidiInput_Stub;
    }

    virtual SoundDriverIOPort_MidiOutput* newMidiOutput(const std::string &name)
    {
        return new(std::nothrow) SoundDriverIOPort_MidiOutput_Stub;
    }
    
    virtual SoundDriverIOPort* findPort(const std::string &name)
    {
        return nullptr;
    }
    
    virtual void deletePort(SoundDriverIOPort* port)
    {
        delete port;
    }

    virtual SoundDriverIOStatusPort* newStatusPort()
    {
        auto port = new(std::nothrow) SoundDriverIOStatusPort_Stub;
        m_status_ports.append(port);
        return port;
    }
    
    virtual void deletePort(SoundDriverIOStatusPort* port)
    {
        auto port_stub = dynamic_cast<SoundDriverIOStatusPort_Stub*>(port);
        if(port_stub)
        {
            m_status_ports.remove(port_stub);
            delete port_stub;
        }
    }
    
    virtual bool connect(const std::string &src, const std::string &dst)
    {
        return true;
    }

    virtual bool disconnect(const std::string &src, const std::string &dst)
    {
        return true;
    }
    
private:
    void* thread()
    {
        while(m_running)
        {
            SoundDriverIOStatus status;
            status.begin_time = status.end_time = current_time();
            for(auto status_port : m_status_ports)
            {
                status_port->buffer.write(&status, 1);
            }
            
            sleep_microseconds(100000);
        }
        return nullptr;
    }
};
    
}//namespace r64fx
