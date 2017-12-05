#include "SoundFileLoader.hpp"
#include "Thread.hpp"
#include "TimeUtils.hpp"
#include "CircularBuffer.hpp"
#include "LinkedList.hpp"
#include "SoundFile.hpp"
#include <string>

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
    Key_AddPort,
    Key_RemovePort,
    Key_Exit,

    Key_OpenFile,
    Key_GetProps,
    Key_LoadChunk,
    Key_FreeChunk,
    Key_CloseFile
};

struct Message_OpenFile{
    std::string              path           = "";
    SoundFileHandle*         handle         = nullptr;
    Callback_OpenSoundFile*  callback       = nullptr;
    void*                    callback_data  = nullptr;

    static inline unsigned long key() { return Key_OpenFile; }
};

struct Message_GetProps{
    SoundFileHandle*                  handle           = nullptr;
    int                               component_count  = 0;
    int                               frame_count      = 0;
    float                             sample_rate      = 0.0f;
    Callback_GetSoundFileProperties*  callback         = nullptr;
    void*                             callback_data    = nullptr;

    static inline unsigned long key() { return Key_GetProps; }
};

struct Message_LoadChunk{
    SoundFileHandle*              handle         = nullptr;
    float*                        chunk          = nullptr;
    int                           index          = 0;
    int                           nframes        = 0;
    Callback_LoadSoundFileChunk*  callback       = nullptr;
    void*                         callback_data  = nullptr;

    static inline unsigned long key() { return Key_LoadChunk; }
};

struct Message_FreeChunk{
    SoundFileHandle*              handle         = nullptr;
    float*                        chunk          = nullptr;
    Callback_FreeSoundFileChunk*  callback       = nullptr;
    void*                         callback_data  = nullptr;

    static inline unsigned long key() { return Key_FreeChunk; }
};

struct Message_CloseFile{
    SoundFileHandle*          handle         = nullptr;
    Callback_CloseSoundFile*  callback       = nullptr;
    void*                     callback_data  = nullptr;

    static inline unsigned long key() { return Key_CloseFile; }
};


struct SoundFileChunk : public LinkedList<SoundFileChunk>::Node{
    float*  ptr   = nullptr;
    long    size  = 0;
};

struct SoundFileRec : public LinkedList<SoundFileRec>::Node{
    SoundFile                   sf;
    std::string                 path        = "";
    LinkedList<SoundFileChunk>  chunks;
    long                        user_count  = 0;
};


class SoundFileLoaderPortImpl : public LinkedList<SoundFileLoaderPortImpl>::Node{
    friend class SoundFileLoaderThread;

    CircularBuffer<Message>*  m_from_iface  = nullptr;
    CircularBuffer<Message>*  m_to_iface    = nullptr;

public:
    SoundFileLoaderPortImpl(CircularBuffer<Message>* from_iface, CircularBuffer<Message>* to_iface)
    : m_from_iface(from_iface)
    , m_to_iface(to_iface)
    {}

private:
    inline int readMessages(Message* msgs, int nmsgs)
    {
        return m_from_iface->read(msgs, nmsgs);
    }

    inline int writeMessages(Message* msgs, int nmsgs)
    {
        return m_to_iface->write(msgs, nmsgs);
    }
};


class SoundFileLoaderThread{
    CircularBuffer<Message>*             m_buffer      = nullptr;
    bool                                 m_running     = false;
    LinkedList<SoundFileLoaderPortImpl>  m_port_impls;
    LinkedList<SoundFileRec>             m_recs;

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
                    case Key_AddPort:
                    {
                        auto port_impl = (SoundFileLoaderPortImpl*) msg.val;
                        m_port_impls.append(port_impl);
                        break;
                    }

                    case Key_RemovePort:
                    {
                        auto port_impl = (SoundFileLoaderPortImpl*) msg.val;
                        m_port_impls.remove(port_impl);
                        delete port_impl;
                        break;
                    }

                    case Key_Exit:
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
                Message msg;
                while(port->readMessages(&msg, 1))
                {
                    switch(msg.key)
                    {
                        case Key_OpenFile:
                        {
                            auto message = (Message_OpenFile*) msg.val;
#ifdef R64FX_DEBUG
                            assert(message != nullptr);
                            assert(!message->path.empty());
#endif//R64FX_DEBUG

                            SoundFileRec* sfrec = nullptr;
                            for(auto rec : m_recs)
                            {
                                if(rec->path == message->path)
                                {
                                    sfrec = rec;
                                    break;
                                }
                            }
                            if(!sfrec)
                            {
                                sfrec = new SoundFileRec;
                                sfrec->sf.open(message->path.c_str(), SoundFile::Mode::Read);
                            }

                            if(sfrec->sf.isGood())
                            {
                                sfrec->user_count++;
                            }

                            if(sfrec->user_count == 0)//Failed to open file!
                            {
                                delete sfrec;
                                sfrec = nullptr;
                            }

                            message->handle = (SoundFileHandle*) sfrec;
                            port->writeMessages(&msg, 1);
                            break;
                        }

                        case Key_GetProps:
                        {
                            auto message = (Message_GetProps*) msg.val;
#ifdef R64FX_DEBUG
                            assert(message != nullptr);
                            assert(message->handle != nullptr);
#endif//R64FX_DEBUG
                            auto sfrec = (SoundFileRec*) message->handle;
#ifdef R64FX_DEBUG
                            assert(sfrec->sf.isGood());
                            assert(sfrec->user_count > 0);
#endif//R64FX_DEBUG
                            message->component_count  = sfrec->sf.componentCount();
                            message->frame_count      = sfrec->sf.frameCount();
                            message->sample_rate      = sfrec->sf.sampleRate();

                            port->writeMessages(&msg, 1);
                            break;
                        }

                        case Key_LoadChunk:
                        {
                            auto message = (Message_LoadChunk*) msg.val;
#ifdef R64FX_DEBUG
                            assert(message != nullptr);
                            assert(message->handle != nullptr);
#endif//R64FX_DEBUG
                            auto sfrec = (SoundFileRec*) message->handle;
#ifdef R64FX_DEBUG
                            assert(sfrec->sf.isGood());
                            assert(sfrec->user_count > 0);
                            assert((message->nframes + message->index) < sfrec->sf.frameCount());
#endif//R64FX_DEBUG
                            sfrec->sf.seek(message->index, SEEK_SET);
                            int chunk_size = sfrec->sf.frameCount() * sfrec->sf.componentCount();
                            message->chunk = new float[chunk_size];
                            sfrec->sf.readFrames(message->chunk, message->nframes);

                            port->writeMessages(&msg, 1);
                            break;
                        }

                        case Key_FreeChunk:
                        {
                            auto message = (Message_LoadChunk*) msg.val;
#ifdef R64FX_DEBUG
                            assert(message != nullptr);
                            assert(message->handle != nullptr);
#endif//R64FX_DEBUG
                            auto sfrec = (SoundFileRec*) message->handle;
#ifdef R64FX_DEBUG
                            assert(sfrec->sf.isGood());
                            assert(sfrec->user_count > 0);
                            assert(message->chunk != nullptr);
#endif//R64FX_DEBUG
                            delete message->chunk;
                            message->chunk = nullptr;
                            port->writeMessages(&msg, 1);
                            break;
                        }

                        case Key_CloseFile:
                        {
                            auto message = (Message_CloseFile*) msg.val;
#ifdef R64FX_DEBUG
                            assert(message != nullptr);
                            assert(message->handle != nullptr);
#endif//R64FX_DEBUG
                            auto sfrec = (SoundFileRec*) message->handle;
#ifdef R64FX_DEBUG
                            assert(sfrec->sf.isGood());
                            assert(sfrec->user_count > 0);
#endif//R64FX_DEBUG
                            sfrec->user_count--;
                            if(sfrec->user_count == 0)
                            {
                                sfrec->sf.close();
                            }

                            port->writeMessages(&msg, 1);
                            break;
                        }

                        default:
                            break;
                    }
                }
            }
            sleep_nanoseconds(500 * 1000);
        }//while

#ifdef R64FX_DEBUG
        assert(m_port_impls.empty());
#endif//R64FX_DEBUG
    }
};



class SoundFileLoaderPortIface{
    friend class SoundFileLoader;

    CircularBuffer<Message>*  m_to_impl    = nullptr;
    CircularBuffer<Message>*  m_from_impl  = nullptr;

    void* m_port_impl = nullptr;

public:
    SoundFileLoaderPortIface(CircularBuffer<Message>* to_impl, CircularBuffer<Message>* from_impl, SoundFileLoaderPortImpl* impl)
    : m_to_impl(to_impl)
    , m_from_impl(from_impl)
    , m_port_impl(impl)
    {}

    inline int writeMessages(Message* msgs, int nmsgs)
    {
        return m_to_impl->write(msgs, nmsgs);
    }

    inline void readMessages()
    {
        Message msg;
        while(m_from_impl->read(&msg, 1))
        {
            switch(msg.key)
            {
                case Key_OpenFile:
                {
                    auto message = (Message_OpenFile*) msg.val;
                    if(message->callback)
                    {
                        message->callback(message->handle, message->callback_data);
                    }
                    delete message;
                    break;
                }

                case Key_GetProps:
                {
                    auto message = (Message_GetProps*) msg.val;
                    if(message->callback)
                    {
                        message->callback(message->handle, message->sample_rate, message->frame_count, message->component_count, message->callback_data);
                    }
                    delete message;
                    break;
                }

                case Key_LoadChunk:
                {
                    auto message = (Message_LoadChunk*) msg.val;
                    if(message->callback)
                    {
                        message->callback(message->handle, message->chunk, message->index, message->nframes, message->callback_data);
                    }
                    delete message;
                    break;
                }

                case Key_FreeChunk:
                {
                    auto message = (Message_FreeChunk*) msg.val;
                    if(message->callback)
                    {
                        message->callback(message->handle, message->callback_data);
                    }
                }

                case Key_CloseFile:
                {
                    auto message = (Message_CloseFile*) msg.val;
                    if(message->callback)
                    {
                        message->callback(message->callback_data);
                    }
                    delete message;
                    break;
                }

                default:
                    break;
            }
        }
    }

    inline void* portImpl() const { return m_port_impl; }
};

}//namespace


#define m_thread       ((Thread*)                   SoundFileLoader_Members[0])
#define m_buffer       ((CircularBuffer<Message>*)  SoundFileLoader_Members[1])
#define m_port_iface   ((SoundFileLoaderPortIface*)                SoundFileLoaderPort_Members)


SoundFileLoader::Port::Port()
{
    
}


SoundFileLoader::Port::~Port()
{
    
}


void SoundFileLoader::Port::run()
{
    m_port_iface->readMessages();
}


void SoundFileLoader::Port::open(const char* file_path, Callback_OpenSoundFile* callback, void* data)
{
    auto message = new Message_OpenFile;
    message->path           = file_path;
    message->callback       = callback;
    message->callback_data  = data;
    Message msg(message->key(), message);
    m_port_iface->writeMessages(&msg, 1);
}


void SoundFileLoader::Port::getFileProperties(SoundFileHandle* handle, Callback_GetSoundFileProperties* callback, void* data)
{
    auto message = new Message_GetProps;
    message->handle         = handle;
    message->callback       = callback;
    message->callback_data  = data;
    Message msg(message->key(), message);
    m_port_iface->writeMessages(&msg, 1);
}


void SoundFileLoader::Port::loadChunk(SoundFileHandle* handle, int index, int nframes, Callback_LoadSoundFileChunk* callback, void* data)
{
    auto message = new Message_LoadChunk;
    message->handle         = handle;
    message->index          = index;
    message->nframes        = nframes;
    message->callback       = callback;
    message->callback_data  = data;
    Message msg(message->key(), message);
    m_port_iface->writeMessages(&msg, 1);
}


void SoundFileLoader::Port::freeChunk(SoundFileHandle* handle, float* chunk, Callback_FreeSoundFileChunk* callback, void* data)
{
    auto message = new Message_FreeChunk;
    message->handle         = handle;
    message->chunk          = chunk;
    message->callback       = callback;
    message->callback_data  = data;
    Message msg(message->key(), message);
    m_port_iface->writeMessages(&msg, 1);
}


void SoundFileLoader::Port::close(SoundFileHandle* handle, Callback_CloseSoundFile* callback, void* data)
{
    auto message = new Message_CloseFile;
    message->handle         = handle;
    message->callback       = callback;
    message->callback_data  = data;
    Message msg(message->key(), message);
    m_port_iface->writeMessages(&msg, 1);
}


SoundFileLoader::SoundFileLoader()
{
    auto thread  = new Thread;
    auto buffer  = new CircularBuffer<Message*>(8);

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
    m_buffer->write(Key_Exit);
    m_thread->join();
    delete m_thread;
    delete m_buffer;
}


SoundFileLoader::Port* SoundFileLoader::newPort()
{
    auto iface_to_impl  = new CircularBuffer<Message>(8);
    auto impl_to_iface  = new CircularBuffer<Message>(8);
    auto port_impl      = new SoundFileLoaderPortImpl(iface_to_impl, impl_to_iface);
    auto port_iface     = new SoundFileLoaderPortIface(iface_to_impl, impl_to_iface, port_impl);

    auto port = new SoundFileLoader::Port;
    port->SoundFileLoaderPort_Members = port_iface;
    m_buffer->write(Message(Key_AddPort, port_impl));

    return port;
}


void SoundFileLoader::deletePort(SoundFileLoader::Port* port)
{
    auto port_iface = (SoundFileLoaderPortIface*) port->SoundFileLoaderPort_Members;
    m_buffer->write(Message(Key_RemovePort, port_iface->portImpl()));
    delete port_iface;
    delete port;
}

}//namespace r64fx
