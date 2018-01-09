#include "SoundDriver.hpp"
#include "LinkedList.hpp"
#include "Debug.hpp"
#include "MemoryUtils.hpp"

/* Used with SoundDriverPortImpl::bits */
#define R64FX_PORT_IS_AUDIO_INPUT   0UL
#define R64FX_PORT_IS_AUDIO_OUTPUT  1UL
#define R64FX_PORT_IS_MIDI_INPUT    2UL
#define R64FX_PORT_IS_MIDI_OUTPUT   3UL
#define R64FX_PORT_IS_OUTPUT        1UL
#define R64FX_PORT_IS_MIDI          2UL
#define R64FX_PORT_OPTION_MASK      3UL


#define R64FX_PORT_GROUP_SYNC_BITS  (0xFFFFFFFFUL << 32)

#define R64FX_INITIAL_STATE         0
#define R64FX_PORT_GROUP_ADD        1
#define R64FX_PORT_GROUP_REMOVE     2
#define R64FX_PORT_GROUP_ENABLE     3
#define R64FX_PORT_GROUP_DISABLE    4
#define R64FX_PORT_UPDATE           5
#define R64FX_PORT_UPDATE_BUFFER    6
#define R64FX_STUB_THREAD_EXIT      7


#define R64FX_MESSAGE_BUFFER_SIZE   32

namespace r64fx{


struct SoundDriverPortImpl : public LinkedList<SoundDriverPortImpl>::Node{
    unsigned long bits    = 0;
    void*         buffer  = nullptr;
};


struct SoundDriverPortGroupImpl : public LinkedList<SoundDriverPortGroupImpl>::Node{
    LinkedList<SoundDriverPortImpl>  ports;
    CircularBuffer<unsigned long>*   from_iface;
    CircularBuffer<unsigned long>*   to_iface;
    unsigned long                    sync_value = 0;
};


template<class ImplBaseT> struct SoundDriverImpl : public ImplBaseT{
    LinkedList<SoundDriverPortGroupImpl>   port_groups;
    CircularBuffer<unsigned long>*         from_iface;
    CircularBuffer<unsigned long>*         to_iface;
    unsigned long                          msgs[R64FX_MESSAGE_BUFFER_SIZE];
    MidiEventBuffer                        midi_input_buffer;

    SoundDriverImpl(
        CircularBuffer<unsigned long>* from_iface,
        CircularBuffer<unsigned long>* to_iface
    )
    : from_iface(from_iface), to_iface(to_iface)
    {
        auto buff = (unsigned int*) alloc_aligned(memory_page_size(), memory_page_size()*4);
        midi_input_buffer = MidiEventBuffer(buff, buff + memory_page_size());
    }

    ~SoundDriverImpl()
    {
        free(midi_input_buffer.begin());
    }

    void process(unsigned int nframes)
    {
        midi_input_buffer.clear();

        unsigned long state = R64FX_INITIAL_STATE;

        int nmsgs = from_iface->read(msgs, R64FX_MESSAGE_BUFFER_SIZE);
        for(int i=0; i<nmsgs; i++)
        {
            switch(state)
            {
                case R64FX_INITIAL_STATE:
                {
                    if(msgs[i] == R64FX_PORT_GROUP_ADD)
                        state = R64FX_PORT_GROUP_ADD;
                    else if(msgs[i] == R64FX_PORT_GROUP_REMOVE)
                        state = R64FX_PORT_GROUP_REMOVE;
                    else if(msgs[i] == R64FX_STUB_THREAD_EXIT)
                        ImplBaseT::exitThread();
                    else
                        R64FX_DEBUG_ABORT("Bad Message!\n");
                    break;
                }

                case R64FX_PORT_GROUP_ADD:
                case R64FX_PORT_GROUP_REMOVE:
                {
                    auto sdpgi = (SoundDriverPortGroupImpl*)msgs[i];
                    if(state == R64FX_PORT_GROUP_ADD)
                    {
                        port_groups.append(sdpgi);
                    }
                    else
                    {
                        port_groups.remove(sdpgi);
                    }
                    unsigned long msgs[2] = { state, (unsigned long)sdpgi };
                    to_iface->write(msgs, 2);
                    state = R64FX_INITIAL_STATE;
                    break;
                }

                default:
                {
                    R64FX_DEBUG_ABORT("Bad State!\n")
                    break;
                }
            }
        }
        R64FX_DEBUG_ASSERT(state == R64FX_INITIAL_STATE);

        SoundDriverPortImpl* updated_port = nullptr;
        for(auto sdpgi : port_groups)
        {
            nmsgs = sdpgi->from_iface->read(msgs, R64FX_MESSAGE_BUFFER_SIZE);

            for(int i=0; i<nmsgs; i++)
            {
                switch(state)
                {
                    case R64FX_INITIAL_STATE:
                    {
                        if(msgs[i] == sdpgi->sync_value)//Got sync message.
                        {
                            for(auto port : sdpgi->ports)
                            {
                                unsigned long option = port->bits & R64FX_PORT_OPTION_MASK;
                                switch(option)
                                {
                                    case R64FX_PORT_IS_AUDIO_INPUT:
                                    {
                                        ImplBaseT::processAudioInput(port, nframes, (float*)port->buffer);
                                        break;
                                    }

                                    case R64FX_PORT_IS_AUDIO_OUTPUT:
                                    {
                                        ImplBaseT::processAudioOutput(port, nframes, (float*)port->buffer);
                                        break;
                                    }

                                    case R64FX_PORT_IS_MIDI_INPUT:
                                    {
                                        auto begin = midi_input_buffer.ptr();
                                        ImplBaseT::processMidiInput(port, nframes, midi_input_buffer);
                                        *((MidiEventBuffer*)port->buffer) = MidiEventBuffer(begin, midi_input_buffer.ptr());
                                        break;
                                    }

                                    case R64FX_PORT_IS_MIDI_OUTPUT:
                                    {
                                        ImplBaseT::processMidiOutput(port, nframes, *((MidiEventBuffer*)port->buffer));
                                        break;
                                    }
                                }
                            }

                            sdpgi->sync_value++;
                            sdpgi->sync_value |= R64FX_PORT_GROUP_SYNC_BITS;
                            sdpgi->to_iface->write(&sdpgi->sync_value, 1);
                        }
                        else if(msgs[i] == R64FX_PORT_GROUP_ENABLE)
                        {
                            sdpgi->sync_value = R64FX_PORT_GROUP_SYNC_BITS;
                            sdpgi->to_iface->write(&sdpgi->sync_value, 1);
                        }
                        else if(msgs[i] == R64FX_PORT_GROUP_DISABLE)
                        {
                            sdpgi->sync_value = 0;
                        }
                        else if(msgs[i] == R64FX_PORT_UPDATE)
                        {
                            state = R64FX_PORT_UPDATE;
                        }
                        break;
                    }

                    case R64FX_PORT_UPDATE:
                    {
                        updated_port = (SoundDriverPortImpl*)msgs[i];
                        state = R64FX_PORT_UPDATE_BUFFER;
                        break;
                    }

                    case R64FX_PORT_UPDATE_BUFFER:
                    {
                        R64FX_DEBUG_ASSERT(updated_port);
                        auto new_buffer = (void*)msgs[i];
                        if(updated_port->buffer && !new_buffer)
                        {
                            sdpgi->ports.remove(updated_port);
                        }
                        else if(!updated_port->buffer && new_buffer)
                        {
                            if(updated_port->bits & R64FX_PORT_IS_OUTPUT)
                                sdpgi->ports.preppend(updated_port);
                            else
                                sdpgi->ports.append(updated_port);
                        }
                        updated_port->buffer = new_buffer;
                        updated_port = nullptr;
                        state = R64FX_INITIAL_STATE;
                        break;
                    }

                    default:
                    {
                        R64FX_DEBUG_ABORT("Bad State!\n")
                        break;
                    }
                }
            }
            if(state != R64FX_INITIAL_STATE)
            {
                std::cout << "state: " << state << "\n";
            }
            R64FX_DEBUG_ASSERT(state == R64FX_INITIAL_STATE);
        }
    }
};


SoundDriverPortGroup::SoundDriverPortGroup()
{
}


void SoundDriverPortGroup::updatePort(SoundDriverPort* port, void* buffer)
{
    R64FX_DEBUG_ASSERT(port);
    unsigned long msgs[3] = {R64FX_PORT_UPDATE, (unsigned long)port, (unsigned long)buffer};
    m_to_impl->write(msgs, 3);
}


void SoundDriverPortGroup::enable()
{
    unsigned long msgs[2] = {R64FX_PORT_GROUP_ENABLE, (unsigned long)this};
    m_to_impl->write(msgs, 2);
}


void SoundDriverPortGroup::disable()
{
    unsigned long msgs[2] = {R64FX_PORT_GROUP_DISABLE, (unsigned long)this};
    m_to_impl->write(msgs, 2);
}


bool SoundDriverPortGroup::sync()
{
    unsigned long msgs[R64FX_MESSAGE_BUFFER_SIZE];
    int nmsgs = m_from_impl->read(msgs, R64FX_MESSAGE_BUFFER_SIZE);
    if(nmsgs > 0)
    {
        auto msg = msgs[nmsgs - 1];
        R64FX_DEBUG_ASSERT((msg & R64FX_PORT_GROUP_SYNC_BITS) == R64FX_PORT_GROUP_SYNC_BITS);
        m_sync_value = msg;
        return true;
    }
    return false;
}


void SoundDriverPortGroup::done()
{
    R64FX_DEBUG_ASSERT(m_sync_value & R64FX_PORT_GROUP_SYNC_BITS);
    m_to_impl->write(&m_sync_value, 1);
}


SoundDriver::SoundDriver()
: m_to_impl(R64FX_MESSAGE_BUFFER_SIZE)
, m_from_impl(R64FX_MESSAGE_BUFFER_SIZE)
{
}


SoundDriver::~SoundDriver()
{
}


SoundDriverPortGroup* SoundDriver::newPortGroup()
{
    auto sdpg = new SoundDriverPortGroup;
    auto sdpgi = new SoundDriverPortGroupImpl;
    sdpg->m_to_impl = sdpgi->from_iface = new CircularBuffer<unsigned long>(R64FX_MESSAGE_BUFFER_SIZE);
    sdpg->m_from_impl = sdpgi->to_iface = new CircularBuffer<unsigned long>(R64FX_MESSAGE_BUFFER_SIZE);
    unsigned long msgs[2] = { R64FX_PORT_GROUP_ADD, (unsigned long)sdpgi };
    m_to_impl.write(msgs, 2);
    sdpg->m_impl = sdpgi;
    m_port_group_count++;
    return sdpg;
}


void SoundDriver::deletePortGroup(SoundDriverPortGroup* sdpg)
{
    unsigned long msgs[2] = { R64FX_PORT_GROUP_REMOVE, (unsigned long)sdpg->m_impl };
    m_to_impl.write(msgs, 2);
    delete sdpg;
}


void SoundDriver::sync()
{
    unsigned long msgs[R64FX_MESSAGE_BUFFER_SIZE];
    int nmsgs = m_from_impl.read(msgs, R64FX_MESSAGE_BUFFER_SIZE);
    unsigned long state = R64FX_INITIAL_STATE;
    for(int i=0; i<nmsgs; i++)
    {
        switch(state)
        {
            case R64FX_INITIAL_STATE:
            {
                if(msgs[i] == R64FX_PORT_GROUP_ADD)
                    state = R64FX_PORT_GROUP_ADD;
                else if(msgs[i] == R64FX_PORT_GROUP_REMOVE)
                    state = R64FX_PORT_GROUP_REMOVE;
                break;
            }

            case R64FX_PORT_GROUP_ADD:
            {
                state = R64FX_INITIAL_STATE;
                break;
            }

            case R64FX_PORT_GROUP_REMOVE:
            {
                auto sdpgi = (SoundDriverPortGroupImpl*)msgs[i];
                delete sdpgi->from_iface;
                delete sdpgi->to_iface;
                delete sdpgi;
                state = R64FX_INITIAL_STATE;
                m_port_group_count--;
                break;
            }
        }
    }
    R64FX_DEBUG_ASSERT(state == R64FX_INITIAL_STATE);
}

}//namespace r64fx


#include "SoundDriver_Stub.cxx"
#ifdef R64FX_USE_JACK
#include "SoundDriver_Jack.cxx"
#endif//R64FX_USE_JACK

namespace r64fx{

SoundDriver* SoundDriver::newInstance(SoundDriver::Type type, const char* client_name)
{
    switch(type)
    {
        case SoundDriver::Type::Default:
        case SoundDriver::Type::Stub:
        {
            return new(std::nothrow) SoundDriver_Stub;
            break;
        }

        case SoundDriver::Type::Jack:
        {
            return new(std::nothrow) SoundDriver_Jack(client_name);
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
