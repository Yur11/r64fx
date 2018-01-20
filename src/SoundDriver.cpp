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

#define R64FX_PORT_GROUP_ADD        0x0
#define R64FX_PORT_GROUP_REMOVE     0x1
#define R64FX_PORT_GROUP_ENABLE     0x2
#define R64FX_PORT_GROUP_DISABLE    0x3
#define R64FX_PORT_UPDATE           0x4
#define R64FX_STUB_THREAD_EXIT      0x5
#define R64FX_PORT_GROUP_KEY_MASK   0x7

#define R64FX_CALLBACK              0x08
#define R64FX_CALLBACK_ARG          0x10


#define R64FX_MESSAGE_BUFFER_SIZE   256

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

        int nmsgs = from_iface->read(msgs, R64FX_MESSAGE_BUFFER_SIZE);
        for(int i=0; i<nmsgs; i++)
        {
            if(msgs[i] == R64FX_PORT_GROUP_ADD)
            {
                int nmsgs_avail = nmsgs - i;
                R64FX_DEBUG_ASSERT(nmsgs_avail >= 2);

                auto sdpgi = (SoundDriverPortGroupImpl*)msgs[i + 1];
                port_groups.append(sdpgi);

                auto nmsgs_written = to_iface->write(msgs + i, 2);
                R64FX_DEBUG_ASSERT(nmsgs_written == 2);
                i++;
            }
            else if(msgs[i] == R64FX_PORT_GROUP_REMOVE)
            {
                int nmsgs_avail = nmsgs - i;
                R64FX_DEBUG_ASSERT(nmsgs_avail >= 2);

                auto sdpgi = (SoundDriverPortGroupImpl*)msgs[i + 1];
                port_groups.remove(sdpgi);

                auto nmsgs_written = to_iface->write(msgs + i, 2);
                R64FX_DEBUG_ASSERT(nmsgs_written == 2);
                i++;
            }
            else if(msgs[i] == R64FX_STUB_THREAD_EXIT)
            {
                ImplBaseT::exitThread();
            }
            else
            {
                R64FX_DEBUG_ABORT("Bad Message!\n");
            }
        }

        for(auto sdpgi : port_groups)
        {
            nmsgs = sdpgi->from_iface->read(msgs, R64FX_MESSAGE_BUFFER_SIZE);

            for(int i=0; i<nmsgs; i++)
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
                    auto nmsgs_written = sdpgi->to_iface->write(&sdpgi->sync_value, 1);
                    R64FX_DEBUG_ASSERT(nmsgs_written == 1);
                }
                else if(msgs[i] == R64FX_PORT_GROUP_ENABLE)
                {
                    sdpgi->sync_value = R64FX_PORT_GROUP_SYNC_BITS;
                    auto nmsgs_written = sdpgi->to_iface->write(&sdpgi->sync_value, 1);
                    R64FX_DEBUG_ASSERT(nmsgs_written == 1);
                }
                else if(msgs[i] == R64FX_PORT_GROUP_DISABLE)
                {
                    sdpgi->sync_value = 0;
                }
                else if((msgs[i] & R64FX_PORT_GROUP_KEY_MASK) == R64FX_PORT_UPDATE)
                {
                    int nmsgs_avail = nmsgs - i;
                    R64FX_DEBUG_ASSERT(nmsgs_avail >= 3);

                    auto updated_port = (SoundDriverPortImpl*)msgs[i + 1];
                    R64FX_DEBUG_ASSERT(updated_port);

                    auto new_buffer = (void*)msgs[i + 2];
                    if(updated_port->buffer && !new_buffer)
                    {
                        sdpgi->ports.remove(updated_port);
                    }
                    else if(!updated_port->buffer && new_buffer)
                    {
                        if(updated_port->bits & R64FX_PORT_IS_OUTPUT)
                        {
                            sdpgi->ports.preppend(updated_port);
                        }
                        else
                        {
                            sdpgi->ports.append(updated_port);
                        }
                    }

                    if(msgs[i] & R64FX_CALLBACK)
                    {
                        R64FX_DEBUG_ASSERT(nmsgs_avail >= 4);
                        unsigned long response_msgs[5] = {msgs[i], (unsigned long)updated_port, (unsigned long)new_buffer, msgs[i + 3], 0};
                        if(msgs[i] & R64FX_CALLBACK_ARG)
                        {
                            R64FX_DEBUG_ASSERT(nmsgs_avail >= 5);
                            response_msgs[4] = msgs[i + 4];
                            sdpgi->to_iface->write(response_msgs, 5);
                            i += 4;
                        }
                        else
                        {
                            sdpgi->to_iface->write(response_msgs, 4);
                            i += 3;
                        }
                    }
                    else
                    {
                        i += 2;
                    }

                    updated_port->buffer = new_buffer;
                    updated_port = nullptr;
                }
                else
                {
                    R64FX_DEBUG_ABORT("Bad Message!\n");
                }
            }//for msgs
        }//for sdpg
    }//process()
};


SoundDriverPortGroup::SoundDriverPortGroup()
{
}


void SoundDriverPortGroup::updatePort(SoundDriverPort* port, void* buffer, PortUpdateCallback* callback, void* arg)
{
    R64FX_DEBUG_ASSERT(port);
    unsigned long msgs[5] = {R64FX_PORT_UPDATE, (unsigned long)port, (unsigned long)buffer, 0, 0};
    int nmsgs = 3;
    if(callback)
    {
        msgs[0] |= R64FX_CALLBACK;
        msgs[3] = (unsigned long)callback;
        nmsgs++;
        if(arg)
        {
            msgs[0] |= R64FX_CALLBACK_ARG;
            msgs[4] = (unsigned long)arg;
            nmsgs++;
        }
    }
    auto nmsgs_written = m_to_impl->write(msgs, nmsgs);
    R64FX_DEBUG_ASSERT(nmsgs_written == nmsgs);
}


void SoundDriverPortGroup::enable()
{
    unsigned long msg = R64FX_PORT_GROUP_ENABLE;
    auto nmsgs = m_to_impl->write(&msg, 1);
    R64FX_DEBUG_ASSERT(nmsgs == 1);
}


void SoundDriverPortGroup::disable()
{
    unsigned long msg = R64FX_PORT_GROUP_DISABLE;
    auto nmsgs = m_to_impl->write(&msg, 1);
    R64FX_DEBUG_ASSERT(nmsgs == 1);
}


bool SoundDriverPortGroup::sync()
{
    unsigned long msgs[R64FX_MESSAGE_BUFFER_SIZE];
    int nmsgs = m_from_impl->read(msgs, R64FX_MESSAGE_BUFFER_SIZE);
    bool result = false;
    for(int i=0; i<nmsgs; i++)
    {
        if((msgs[i] & R64FX_PORT_GROUP_SYNC_BITS) == R64FX_PORT_GROUP_SYNC_BITS)
        {
            m_sync_value = msgs[i];
            result = true;
        }
        else if((msgs[i] & R64FX_PORT_GROUP_KEY_MASK) == R64FX_PORT_UPDATE)
        {
            int nmsgs_avail = nmsgs - i;
            R64FX_DEBUG_ASSERT(nmsgs_avail >= 4);
            R64FX_DEBUG_ASSERT(msgs[i] & R64FX_CALLBACK);
            auto port      = (SoundDriverPort*)    msgs[i + 1];
            auto buffer    = (void*)               msgs[i + 2];
            auto callback  = (PortUpdateCallback*) msgs[i + 2];
            void* arg = nullptr;
            if(msgs[i] & R64FX_CALLBACK_ARG)
            {
                R64FX_DEBUG_ASSERT(nmsgs_avail >= 5);
                arg = (void*)msgs[i];
                i += 4;
            }
            else
            {
                i += 3;
            }
            R64FX_DEBUG_ASSERT(callback);
            R64FX_DEBUG_ASSERT(port);
            callback(port, buffer, arg);
        }
        else
        {
            R64FX_DEBUG_ABORT("Bad Message!\n");
        }
    }
    return result;
}


void SoundDriverPortGroup::done()
{
    R64FX_DEBUG_ASSERT(m_sync_value & R64FX_PORT_GROUP_SYNC_BITS);
    auto nmsgs = m_to_impl->write(&m_sync_value, 1);
    R64FX_DEBUG_ASSERT(nmsgs == 1);
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
    auto nmsgs = m_to_impl.write(msgs, 2);
    R64FX_DEBUG_ASSERT(nmsgs == 2);
    sdpg->m_impl = sdpgi;
    m_port_group_count++;
    return sdpg;
}


void SoundDriver::deletePortGroup(SoundDriverPortGroup* sdpg)
{
    unsigned long msgs[2] = {R64FX_PORT_GROUP_REMOVE, (unsigned long)sdpg->m_impl};
    auto nmsgs = m_to_impl.write(msgs, 2);
    R64FX_DEBUG_ASSERT(nmsgs == 2);
    delete sdpg;
}


void SoundDriver::sync()
{
    unsigned long msgs[R64FX_MESSAGE_BUFFER_SIZE];
    int nmsgs = m_from_impl.read(msgs, R64FX_MESSAGE_BUFFER_SIZE);
    for(int i=0; i<nmsgs; i++)
    {
        if(msgs[i] == R64FX_PORT_GROUP_ADD)
        {
            i++;
        }
        else if(msgs[i] == R64FX_PORT_GROUP_REMOVE)
        {
            int nmsgs_avail = nmsgs - i;
            R64FX_DEBUG_ASSERT(nmsgs_avail >= 2);

            auto sdpgi = (SoundDriverPortGroupImpl*)msgs[i + 1];
            delete sdpgi->from_iface;
            delete sdpgi->to_iface;
            delete sdpgi;
            m_port_group_count--;
            i++;
        }
        else
        {
            std::cout << msgs[i] << "\n";
            R64FX_DEBUG_ABORT("Bad Message!\n");
        }
    }
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
