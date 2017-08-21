#include "SoundFileLoader.hpp"
#include "Thread.hpp"
#include "CircularBuffer.hpp"
#include "LinkedList.hpp"
#include "SoundFile.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

struct Message{
    unsigned long key = 0;
    unsigned long val = 0;

    Message()
    {}

    Message(unsigned long key)
    : key(key)
    {}

    Message(unsigned long key, unsigned long val)
    : key(key)
    , val(val)
    {}

    Message(unsigned long key, void* val)
    : key(key)
    , val((unsigned long)val)
    {}
};

enum{
    AddPort,
    RemovePort,
    Exit,

    Open,
    GetFileProperties,
    Close,
};



struct SoundFileRec : public LinkedList<SoundFileRec>::Node{
    SoundFile sf;
};


class PortImpl : public LinkedList<PortImpl>::Node{
    friend class SoundFileLoaderThread;

    CircularBuffer<Message>*  m_from_iface  = nullptr;
    CircularBuffer<Message>*  m_to_iface    = nullptr;

    LinkedList<SoundFileRec> m_recs;

public:
    PortImpl(CircularBuffer<Message>* from_iface, CircularBuffer<Message>* to_iface)
    : m_from_iface(from_iface)
    , m_to_iface(to_iface)
    {}

private:
    inline void readMessages()
    {
        Message msg;
        while(m_from_iface->read(&msg, 1))
        {
            switch(msg.key)
            {
                default:
                    break;
            }
        }
    }
};


class SoundFileLoaderThread{
    CircularBuffer<Message>* m_buffer = nullptr;
    bool m_running = false;
    LinkedList<PortImpl> m_port_impls;

public:
    SoundFileLoaderThread(CircularBuffer<Message>* buffer)
    : m_buffer(buffer)
    {}

    void run()
    {
        m_running = true;
        while(m_running)
        {
            Message msg;
            while(m_buffer->read(&msg, 1))
            {
                switch(msg.key)
                {
                    case AddPort:
                    {
                        auto port_impl = (PortImpl*) msg.val;
                        m_port_impls.append(port_impl);
                        break;
                    }

                    case RemovePort:
                    {
                        auto port_impl = (PortImpl*) msg.val;
                        m_port_impls.remove(port_impl);
                        delete port_impl;
                        break;
                    }

                    case Exit:
                    {
                        m_running = false;
                        break;
                    }

                    default:
                        break;
                }//switch
            }//while

            for(auto port : m_port_impls)
            {
                port->readMessages();
            }
        }//while

#ifdef R64FX_DEBUG
        assert(m_port_impls.isEmpty());
#endif//R64FX_DEBUG
    }
};



class PortIface{
    friend class SoundFileLoader;

    CircularBuffer<Message>*  m_to_impl     = nullptr;
    CircularBuffer<Message>*  m_from_impl  = nullptr;

public:
    PortIface(CircularBuffer<Message>* to_impl, CircularBuffer<Message>* from_impl)
    : m_to_impl(to_impl)
    , m_from_impl(from_impl)
    {}

    inline void readMessages()
    {
        Message msg;
        while(m_from_impl->read(&msg, 1))
        {
            switch(msg.key)
            {
                default:
                    break;
            }
        }
    }
};

}//namespace


#define m_thread       ((Thread*)                   SoundFileLoader_Members[0])
#define m_buffer       ((CircularBuffer<Message>*)  SoundFileLoader_Members[1])
#define m_port_iface   ((PortIface*)                SoundFileLoaderPort_Members)


SoundFileLoader::Port::Port()
{
    
}


SoundFileLoader::Port::~Port()
{
    
}


void SoundFileLoader::Port::readMessages()
{
    m_port_iface->readMessages();
}


SoundFileHandle* SoundFileLoader::Port::open(const char* file_path)
{
    return nullptr;
}


void SoundFileLoader::Port::getFileProperties(SoundFileHandle* handle, void (*callback)(float sample_rate, int frame_count, int component_count))
{
    
}


void SoundFileLoader::Port::loadChunk(SoundFileHandle* handle, int index, int nframes, void (*callback)(float* chunk, int index, int nframes))
{
    
}


void SoundFileLoader::Port::close(SoundFileHandle* handle)
{
    
}


SoundFileLoader::SoundFileLoader()
{
    auto thread  = new Thread;
    auto buffer  = new CircularBuffer<Message*>(16);

    thread->run([](void* arg) -> void*{
        SoundFileLoaderThread sflt((CircularBuffer<Message>*) arg);
        sflt.run();
        return nullptr;
    }, buffer);

    SoundFileLoader_Members[0] = thread;
    SoundFileLoader_Members[1] = buffer;
}


SoundFileLoader::~SoundFileLoader()
{
    m_buffer->write(Message(Exit));
    m_thread->join();
    delete m_thread;
    delete m_buffer;
}


SoundFileLoader::Port* SoundFileLoader::newPort()
{
    auto iface_to_impl  = new CircularBuffer<Message>(16);
    auto impl_to_iface  = new CircularBuffer<Message>(16);
    auto port_iface     = new PortIface(iface_to_impl, impl_to_iface);
    auto port_impl      = new PortImpl(iface_to_impl, impl_to_iface);

    auto port = new SoundFileLoader::Port;
    port->SoundFileLoaderPort_Members = port_iface;
    m_buffer->write(Message(AddPort, port_impl));

    return port;
}


void SoundFileLoader::freePort(SoundFileLoader::Port* port)
{
    delete port;
}

}//namespace r64fx
