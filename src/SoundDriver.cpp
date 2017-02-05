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

class PortImplHandle;
class PortIfaceHandle;


template<typename T> struct PortTypeTraits{
};


template<> struct PortTypeTraits<float>{
    static const SoundDriverPort::Type  type()  { return SoundDriverPort::Type::Audio; }
    static const unsigned long          flags() { return R64FX_PORT_IS_AUDIO; }
};

template<> struct PortTypeTraits<MidiEvent>{
    static const SoundDriverPort::Type  type()  { return SoundDriverPort::Type::Midi; }
    static const unsigned long          flags() { return R64FX_PORT_IS_MIDI; }
};


template<typename T, typename HandleT>  class PortImpl : public LinkedList<PortImpl<T, HandleT>>::Node{
    HandleT*            m_handle  = nullptr;
    PortIfaceHandle*    m_iface   = nullptr;

protected:
    CircularBuffer<T>*  m_buffer  = nullptr;
    unsigned long       m_flags   = 0;

public:
    PortImpl(HandleT* handle, PortIfaceHandle* iface, CircularBuffer<T>* buffer, unsigned long flags) 
    : m_handle(handle)
    , m_iface(iface)
    , m_buffer(buffer)
    , m_flags(flags)
    {
        
    }

    inline HandleT* handle() const { return m_handle; }

    inline PortIfaceHandle* iface() const { return m_iface; }

    inline unsigned long flags() const;
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
    inline int read(T* items, int nitems)
    {
        return this->m_buffer->read(items, nitems);
    }
};


template<typename T, typename HandleT>  class PortIface : public LinkedList<PortIface<T, HandleT>>::Node{
    HandleT*         m_handle  = nullptr;
    PortImplHandle*  m_impl    = nullptr;

protected:
    CircularBuffer<T>*  m_buffer  = nullptr;

    inline void setImpl(PortImplHandle* impl) { m_impl = impl; } 

public:
    PortIface(HandleT* handle, int nitems)
    : m_handle(handle)
    {
        m_buffer = new(std::nothrow) CircularBuffer<T>(nitems);
    }

    ~PortIface()
    {
        delete m_buffer;
    }

    inline HandleT* handle() const { return m_handle; }

    inline PortImplHandle* impl() const { return m_impl; }
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
        auto new_impl = (PortIfaceHandle*) new(std::nothrow) InputPortImpl<T, HandleT>(
            this->handle(), 
            (PortIfaceHandle*)this, 
            this->m_buffer, 
            R64FX_PORT_IS_INPUT | PortTypeTraits<T>::flags()
        );
        if(new_impl)
            this->setImpl(new_impl);
        return new_impl;
    }
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
        auto new_impl = (PortIfaceHandle*) new(std::nothrow) OutputPortImpl<T, HandleT>(
            this->handle(), 
            (PortIfaceHandle*)this, 
            this->m_buffer, 
            R64FX_PORT_IS_OUTPUT | PortTypeTraits<T>::flags()
        );
        if(new_impl)
            this->setImpl(new_impl);
        return new_impl;
    }
};

#define R64FX_PORT_GUTS(TYPE, DIRECTION, OPERATION, ITEMS)\
using DIRECTION##PortIface<TYPE, HandleT>::InputPortIface;\
\
virtual SoundDriverPort::Type type()\
{\
    return SoundDriverPort::Type::TYPE;\
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


#define R64FX_SOUND_DRIVER_PORT_CLASSES(Prefix, HandleT)\
struct Prefix##AudioInput : public SoundDriverAudioInput,  public InputPortIface  <float,     HandleT> { R64FX_PORT_GUTS(Input,  float,     read,  Samples) };\
struct Prefix##AudioInput : public SoundDriverAudioOutput, public OutputPortIface <float,     HandleT> { R64FX_PORT_GUTS(Output, float,     write, Samples) };\
struct Prefix##MidiInput  : public SoundDriverMidiInput,   public InputPortIface  <MidiEvent, HandleT> { R64FX_PORT_GUTS(Input,  MidiEvent, read,  Samples) };\
struct Prefix##MidiInput  : public SoundDriverMidiOutput,  public OutputPortIface <MidiEvent, HandleT> { R64FX_PORT_GUTS(Output, MidiEvent, write, Samples) };\


#include "SoundDriver_Stub.cxx"
#ifdef R64FX_USE_JACK
// #include "SoundDriver_Jack.cxx"
#endif//R64FX_USE_JACK


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

//         case SoundDriver::Type::Jack:
//         {
//             return new(std::nothrow) Jack;
//         }

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
