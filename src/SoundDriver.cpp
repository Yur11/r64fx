#include "SoundDriver.hpp"
#include "CircularBuffer.hpp"
#include "LinkedList.hpp"
#include <new>
#include <iostream>

#define R64FX_PORT_IS_INPUT         0
#define R64FX_PORT_IS_OUTPUT        1

#define R64FX_PORT_IS_AUDIO         (0 << 1)
#define R64FX_PORT_IS_MIDI          (1 << 1)

#define R64FX_PORT_IS_AUDIO_INPUT   0
#define R64FX_PORT_IS_AUDIO_OUTPUT  1
#define R64FX_PORT_IS_MIDI_INPUT    2
#define R64FX_PORT_IS_MIDI_OUTPUT   3

#define R64FX_PORT_OPTION_MASK      3


namespace r64fx{

/* No definitions. Using these to store pointers. */
class PortImplHandle;
class PortIfaceHandle;
class SyncPortImplHandle;
class SyncPortIfaceHandle;


template<typename> struct PortTypeTraits{
};


template<> struct PortTypeTraits<float>{
    static const SoundDriverPort::Type  type()  { return SoundDriverPort::Type::Audio; }
    static const unsigned long          flags() { return R64FX_PORT_IS_AUDIO; }
};

template<> struct PortTypeTraits<MidiEvent>{
    static const SoundDriverPort::Type  type()  { return SoundDriverPort::Type::Midi; }
    static const unsigned long          flags() { return R64FX_PORT_IS_MIDI; }
};


template<typename HandleT> class BasePortImpl : public LinkedList<BasePortImpl<HandleT>>::Node{
    HandleT           m_handle  = nullptr;
    PortIfaceHandle*  m_iface   = nullptr;

protected:
    unsigned long  m_flags   = 0;
    
public:
    BasePortImpl(HandleT handle, PortIfaceHandle* iface, unsigned long flags) : m_handle(handle), m_iface(iface), m_flags(flags) {}

    inline HandleT  handle() const { return m_handle; }

    inline PortIfaceHandle* iface() const { return m_iface; }

    inline unsigned long flags() const { return m_flags; }
};


template<typename T, typename HandleT>  class PortImpl : public BasePortImpl<HandleT>{
protected:
    CircularBuffer<T>*  m_buffer  = nullptr;

public:
    PortImpl(HandleT handle, PortIfaceHandle* iface, unsigned long flags, CircularBuffer<T>* buffer) 
    : BasePortImpl<HandleT>(handle, iface, flags)
    , m_buffer(buffer)
    {
        
    }
};


template<typename T, typename HandleT>  class InputPortImpl : public PortImpl<T, HandleT>{
public:
    using PortImpl<T, HandleT>::PortImpl;

    inline int write(T* items, int nitems)
    {
        return this->m_buffer->write(items, nitems);
    }
};


template<typename T, typename HandleT>  class OutputPortImpl : public PortImpl<T, HandleT>{
public:
    using PortImpl<T, HandleT>::PortImpl;

    inline int read(T* items, int nitems)
    {
        return this->m_buffer->read(items, nitems);
    }
};


template<typename HandleT> class BasePortIface : public LinkedList<BasePortIface<HandleT>>::Node{
    HandleT          m_handle  = nullptr;
    PortImplHandle*  m_impl    = nullptr;

protected:
    inline void setImpl(PortImplHandle* impl) { m_impl = impl; } 

public:
    BasePortIface(HandleT handle) : m_handle(handle) {}

    inline HandleT handle() const { return m_handle; }

    inline PortImplHandle* impl() const { return m_impl; }
};


template<typename T, typename HandleT>  class PortIface : public BasePortIface<HandleT>{
protected:
    CircularBuffer<T>*  m_buffer  = nullptr;

public:
    PortIface(HandleT handle, int nitems)
    : BasePortIface<HandleT>(handle)
    {
        m_buffer = new(std::nothrow) CircularBuffer<T>(nitems);
    }

    ~PortIface()
    {
        delete m_buffer;
    }
};


template<typename T, typename HandleT> class InputPortIface : public PortIface<T, HandleT>{
public:
    using PortIface<T, HandleT>::PortIface;

    inline int read(T* items, int nitems)
    {
        return this->m_buffer->read(items, nitems);
    }

    inline PortImplHandle* newImpl()
    {
#ifdef R64FX_DEBUG
        assert(this->impl() == nullptr);
#endif//R64FX_DEBUG
        auto new_impl = (PortImplHandle*) new(std::nothrow) InputPortImpl<T, HandleT>(
            this->handle(), (PortIfaceHandle*)this,
            R64FX_PORT_IS_INPUT | PortTypeTraits<T>::flags(),
            this->m_buffer
        );
        if(new_impl)
            this->setImpl(new_impl);
        return new_impl;
    }

    inline static const unsigned long portDirectionFlag() { return R64FX_PORT_IS_AUDIO_INPUT; }
};


template<typename T, typename HandleT> class OutputPortIface : public PortIface<T, HandleT>{
public:
    using PortIface<T, HandleT>::PortIface;

    inline int write(T* items, int nitems)
    {
        return this->m_buffer->write(items, nitems);
    }

    inline PortImplHandle* newImpl()
    {
#ifdef R64FX_DEBUG
        assert(this->impl() == nullptr);
#endif//R64FX_DEBUG
        auto new_impl = (PortImplHandle*) new(std::nothrow) OutputPortImpl<T, HandleT>(
            this->handle(), (PortIfaceHandle*)this, 
            R64FX_PORT_IS_OUTPUT | PortTypeTraits<T>::flags(),
            this->m_buffer
        );
        if(new_impl)
            this->setImpl(new_impl);
        return new_impl;
    }

    inline static const unsigned long portDirectionFlag() { return R64FX_PORT_IS_AUDIO_OUTPUT; }
};


#define R64FX_PORT_GUTS(DIRECTION, TYPE, OPERATION, ITEMS, H)\
using DIRECTION##PortIface<TYPE, H>::DIRECTION##PortIface;\
\
virtual SoundDriverPort::Type type()\
{\
    return PortTypeTraits<TYPE>::type();\
}\
\
virtual SoundDriverPort::Direction direction()\
{\
    return SoundDriverPort::Direction::DIRECTION;\
}\
\
virtual int OPERATION##ITEMS(TYPE* items, int nitems)\
{\
    return OPERATION(items, nitems);\
}


#define R64FX_SOUND_DRIVER_PORT_CLASSES(PREFIX, H)\
struct PREFIX##AudioInput  : public SoundDriverAudioInput,  public InputPortIface  <float,     H> { R64FX_PORT_GUTS(Input,  float,     read,  Samples, H) };\
struct PREFIX##AudioOutput : public SoundDriverAudioOutput, public OutputPortIface <float,     H> { R64FX_PORT_GUTS(Output, float,     write, Samples, H) };\
struct PREFIX##MidiInput   : public SoundDriverMidiInput,   public InputPortIface  <MidiEvent, H> { R64FX_PORT_GUTS(Input,  MidiEvent, read,  Events,  H) };\
struct PREFIX##MidiOutput  : public SoundDriverMidiOutput,  public OutputPortIface <MidiEvent, H> { R64FX_PORT_GUTS(Output, MidiEvent, write, Events,  H) };\



class SyncPortImpl : public LinkedList<SyncPortImpl>::Node{
    SyncPortIfaceHandle*                     m_iface       = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  m_to_iface    = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  m_from_iface  = nullptr;

public:
    SyncPortImpl(SyncPortIfaceHandle* iface, CircularBuffer<SoundDriverSyncMessage>* to_iface, CircularBuffer<SoundDriverSyncMessage>* from_iface)
    : m_iface((SyncPortIfaceHandle*)iface)
    , m_to_iface(to_iface)
    , m_from_iface(from_iface)
    {
    }

    ~SyncPortImpl()
    {
        delete m_to_iface;
        delete m_from_iface;
    }

    inline SyncPortIfaceHandle* iface() const { return m_iface; }

    inline int writeMessages(SoundDriverSyncMessage* msgs, int nmsgs)
    {
        return m_to_iface->write(msgs, nmsgs);
    }

    inline int readMessages(SoundDriverSyncMessage* msgs, int nmsgs)
    {
        return m_from_iface->read(msgs, nmsgs);
    }
};


class SyncPortIface : public SoundDriverSyncPort, public LinkedList<SyncPortIface>::Node{
    SyncPortImplHandle*                      m_impl       = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  m_to_impl    = nullptr;
    CircularBuffer<SoundDriverSyncMessage>*  m_from_impl  = nullptr;

public:
    SyncPortIface(int nitems)
    {
        m_to_impl = new(std::nothrow) CircularBuffer<SoundDriverSyncMessage>(nitems);
        m_from_impl = new(std::nothrow) CircularBuffer<SoundDriverSyncMessage>(nitems);
    }

    ~SyncPortIface()
    {
        if(m_impl)
            delete (SyncPortImpl*) m_impl;

        if(m_to_impl)
            delete m_to_impl;

        if(m_from_impl)
            delete m_from_impl;
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

    virtual int writeMessages(SoundDriverSyncMessage* msgs, int nmsgs)
    {
        return m_to_impl->read(msgs, nmsgs);
    }

    virtual int readMessages(SoundDriverSyncMessage* msgs, int nmsgs)
    {
        return m_from_impl->read(msgs, nmsgs);
    }

    inline SyncPortImplHandle* impl() const
    {
        return m_impl;
    }

    inline SyncPortImplHandle* newImpl()
    {
#ifdef R64FX_DEBUG
        assert(m_impl == nullptr);
#endif//R64FX_DEBUG
        m_impl = (SyncPortImplHandle*) new(std::nothrow) SyncPortImpl((SyncPortIfaceHandle*)this, m_from_impl, m_to_impl);
        return m_impl;
    }
};


class SoundDriverMessage{
    unsigned long m_key    = 0;
    void* m_value  = 0;

public:
    SoundDriverMessage(){}

    SoundDriverMessage(unsigned long key, void* value)
    : m_key(key)
    , m_value(value)
    {}

    inline unsigned long key() const { return m_key; }

    inline void* value() const { return m_value; }
};


enum{
    AddPort,
    RemovePort,
    AddSyncPort,
    RemoveSyncPort
};

enum{
    PortAdded,
    PortRemoved,
    SyncPortAdded,
    SyncPortRemoved
};


template<typename HandleT> class SoundDriverThreadImpl{
    LinkedList<BasePortImpl<HandleT>>    m_ports;
    LinkedList<SyncPortImpl>             m_sync_ports;
    CircularBuffer<SoundDriverMessage>*  m_from_iface  = nullptr;
    CircularBuffer<SoundDriverMessage>*  m_to_iface    = nullptr;

protected:
    inline const LinkedList<BasePortImpl<HandleT>> &ports() const { return m_ports; }

public:
    SoundDriverThreadImpl(CircularBuffer<SoundDriverMessage>* to_impl, CircularBuffer<SoundDriverMessage>* from_impl)
    : m_from_iface(to_impl)
    , m_to_iface(from_impl)
    {
        
    }

protected:
    void prologue()
    {
        SoundDriverMessage msg;
        while(m_from_iface->read(&msg, 1))
        {
            switch(msg.key())
            {
                case AddPort:
                {
                    msgAddPort((BasePortImpl<HandleT>*) msg.value());
                    break;
                }

                case RemovePort:
                {
                    msgRemovePort((BasePortImpl<HandleT>*) msg.value());
                    break;
                }

                case AddSyncPort:
                {
                    msgAddSyncPort((SyncPortImpl*) msg.value());
                    break;
                }

                case RemoveSyncPort:
                {
                    msgRemoveSyncPort((SyncPortImpl*) msg.value());
                    break;
                }
            }
        }
    }

    void epilogue()
    {
        
    }
    
private:
    inline void msgAddPort(BasePortImpl<HandleT>* port_impl)
    {
        m_ports.append(port_impl);
        SoundDriverMessage msg(PortAdded, port_impl->iface());
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    inline void msgRemovePort(BasePortImpl<HandleT>* port_impl)
    {
        m_ports.remove(port_impl);
        SoundDriverMessage msg(PortRemoved, port_impl->iface());
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    inline void msgAddSyncPort(SyncPortImpl* port_impl)
    {
        m_sync_ports.append(port_impl);
        SoundDriverMessage msg(SyncPortAdded, port_impl->iface());
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }

    inline void msgRemoveSyncPort(SyncPortImpl* port_impl)
    {
        m_sync_ports.remove(port_impl);
        SoundDriverMessage msg(SyncPortRemoved, port_impl->iface());
        int nwritten = m_to_iface->write(&msg, 1);
#ifdef R64FX_DEBUG
        assert(nwritten == 1);
#endif//R64FX_DEBUG
    }
};


template<typename HandleT> class SoundDriverPartial : public SoundDriver{
    CircularBuffer<SoundDriverMessage>*  m_to_impl      = nullptr;
    CircularBuffer<SoundDriverMessage>*  m_from_impl    = nullptr;
    LinkedList<BasePortIface<HandleT>>   m_ports;
    LinkedList<SyncPortIface>            m_sync_ports;

protected:
    SoundDriverPartial()
    {
        m_to_impl   = new(std::nothrow) CircularBuffer<SoundDriverMessage>(16);
        m_from_impl = new(std::nothrow) CircularBuffer<SoundDriverMessage>(16);
    }

    ~SoundDriverPartial()
    {
        if(m_to_impl)
            delete m_to_impl;

        if(m_from_impl)
            delete m_from_impl;
    }

    inline CircularBuffer<SoundDriverMessage>* toImpl()   const { return m_to_impl; }
    inline CircularBuffer<SoundDriverMessage>* fromImpl() const { return m_from_impl; }

    template<typename PortT> bool addPort(PortT* port)
    {
        auto impl = port->newImpl();
        if(!impl)
            return false;
        SoundDriverMessage msg(AddPort, impl);
        m_to_impl->write(&msg, 1);
        m_ports.append(port);
        return true;
    }

    void removePort(BasePortIface<HandleT>* port)
    {
        SoundDriverMessage msg(AddPort, port->impl());
        m_to_impl->write(&msg, 1);
        m_ports.remove(port);
    }

    virtual void portAdded(BasePortIface<HandleT>* port)
    {
        
    }

    virtual void portRemoved(BasePortIface<HandleT>* port)
    {
        
    }

    
private:
    virtual SoundDriverSyncPort* newSyncPort()
    {
        auto port_iface = new(std::nothrow) SyncPortIface(16);
        auto port_impl = port_iface->newImpl();
        if(!port_impl)
        {
            delete port_iface;
            return nullptr;
        }

        SoundDriverMessage msg(AddSyncPort, port_impl);
        m_to_impl->write(&msg, 1);
        m_sync_ports.append(port_iface);

        return port_iface;
    }

    virtual void deleteSyncPort(SoundDriverSyncPort* port)
    {
        auto port_iface = dynamic_cast<SyncPortIface*>(port);
#ifdef R64FX_DEBUG
        assert(port_iface != nullptr);
#endif//R64FX_DEBUG
        SoundDriverMessage msg(RemoveSyncPort, port_iface->impl());
        m_sync_ports.remove(port_iface);
    }

    virtual void processEvents()
    {
        SoundDriverMessage msg;
        while(m_from_impl->read(&msg, 1))
        {
            switch(msg.key())
            {
                case PortAdded:
                {
                    portAdded((BasePortIface<HandleT>*) msg.value());
                    break;
                }

                case PortRemoved:
                {
                    portRemoved((BasePortIface<HandleT>*) msg.value());
                    break;
                }

                case SyncPortAdded:
                {
                    break;
                }

                case SyncPortRemoved:
                {
                    auto sync_port = (SoundDriverSyncPort*) msg.value();
                    delete sync_port;
                    break;
                }
            }
        }
    }
};

}//namespace r64fx


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
        }
    }
}


void SoundDriver::deleteInstance(SoundDriver* driver)
{
    delete driver;
}

}//namespace r64fx
