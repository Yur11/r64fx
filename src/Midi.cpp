#include "Midi.hpp"

namespace r64fx{

MidiMessage::Type chan_mode(MidiMessage msg)
{
    switch(msg.byte(1))
    {
        case 120:
            return MidiMessage::Type::AllSoundOff;

        case 121:
            return MidiMessage::Type::ResetAllCtrls;

        case 122:
        {
            if(msg.byte(1) == 0)
                return MidiMessage::Type::LocalControlOff;
            else if(msg.byte(1) == 127)
                return MidiMessage::Type::LocalControlOn;
            else
                return MidiMessage::Type::Bad;
        }

        case 123:
            return MidiMessage::Type::AllNotesOff;

        case 124:
            return MidiMessage::Type::OmniOff;

        case 125:
            return MidiMessage::Type::OmniOn;

        case 126:
            return MidiMessage::Type::MonoMode;

        case 127:
            return MidiMessage::Type::PolyMode;

        default:
            return MidiMessage::Type::Bad;
    }
}


MidiMessage::Type system_common(MidiMessage msg)
{
    switch(msg.byte(0))
    {
        case 0xF0:
            return MidiMessage::Type::SysEx;

        case 0xF1:
            return MidiMessage::Type::TimeCode;

        case 0xF2:
            return MidiMessage::Type::SongPosition;

        case 0xF3:
            return MidiMessage::Type::SysEx;

        case 0xF4:
        case 0xF5:
            return MidiMessage::Type::Undefined;

        case 0xF6:
            return MidiMessage::Type::TuneRequest;

        case 0xF7:
            return MidiMessage::Type::SysExEnd;

        case 0xF8:
            return MidiMessage::Type::Clock;

        case 0xF9:
            return MidiMessage::Type::Undefined;

        case 0xFA:
            return MidiMessage::Type::Start;

        case 0xFB:
            return MidiMessage::Type::Continue;

        case 0xFC:
            return MidiMessage::Type::Stop;

        case 0xFD:
            return MidiMessage::Type::Undefined;

        case 0xFE:
            return MidiMessage::Type::ActiveSensing;

        case 0xFF:
            return MidiMessage::Type::Reset;

        default:
            return MidiMessage::Type::Bad;
    }
}


MidiMessage::Type MidiMessage::type() const
{
    switch(byte(0) & 0xF0)
    {
        case 0x80:
            return MidiMessage::Type::NoteOff;

        case 0x90:
            return MidiMessage::Type::NoteOn;

        case 0xA0:
            return MidiMessage::Type::PolyAft;

        case 0xB0:
        {
            if(byte(1) < 120)
                return MidiMessage::Type::ControlChange;
            else
                return chan_mode(*this);
        }

        case 0xC0:
            return MidiMessage::Type::ProgramChange;

        case 0xD0:
            return MidiMessage::Type::ChanAft;

        case 0xE0:
            return MidiMessage::Type::PitchBend;

        case 0xF0:
            return system_common(*this);

        default:
            return MidiMessage::Type::Bad;
    }
}


int MidiMessage::byteCount() const
{
    switch(type())
    {
        case MidiMessage::Type::NoteOff:
        case MidiMessage::Type::NoteOn:
        case MidiMessage::Type::PolyAft:
        case MidiMessage::Type::ControlChange:
            return 3;

        case MidiMessage::Type::ProgramChange:
        case MidiMessage::Type::ChanAft:
            return 2;

        case MidiMessage::Type::PitchBend:
        case MidiMessage::Type::AllSoundOff:
        case MidiMessage::Type::ResetAllCtrls:
        case MidiMessage::Type::LocalControlOff:
        case MidiMessage::Type::LocalControlOn:
        case MidiMessage::Type::AllNotesOff:
        case MidiMessage::Type::OmniOff:
        case MidiMessage::Type::OmniOn:
        case MidiMessage::Type::MonoMode:
        case MidiMessage::Type::PolyMode:
        case MidiMessage::Type::SysEx:
            return 3;

        case MidiMessage::Type::TimeCode:
            return 2;

        case MidiMessage::Type::SongPosition:
            return 3;

        case MidiMessage::Type::SongSelect:
            return 2;

        case MidiMessage::Type::TuneRequest:
        case MidiMessage::Type::SysExEnd:
        case MidiMessage::Type::Clock:
        case MidiMessage::Type::Start:
        case MidiMessage::Type::Continue:
        case MidiMessage::Type::Stop:
        case MidiMessage::Type::ActiveSensing:
        case MidiMessage::Type::Reset:
            return 1;

        default:
            return -1;
    }
}


bool MidiEventBuffer::read(MidiEvent &event)
{
    if(m_ptr >= m_end)
        return false;

    auto dword = *m_ptr;
    m_ptr++;
    if((dword & 0xFF000000) == 0xFF000000)
    {
        R64FX_DEBUG_ASSERT(m_ptr < m_end);
        m_time = dword & 0x00FFFFFF;
        dword = *m_ptr;
        R64FX_DEBUG_ASSERT((dword & 0xFF000000) == 0);
        m_ptr++;
    }
    else
    {
        R64FX_DEBUG_ASSERT(m_ptr <= m_end);
        m_time += (dword >> 24);
    }
    event = MidiEvent(MidiMessage(dword & 0x00FFFFFF), m_time);
    return true;
}


bool MidiEventBuffer::write(MidiEvent event)
{
    R64FX_DEBUG_ASSERT(event.time >= m_time);
    R64FX_DEBUG_ASSERT(event.time <= 0x00FFFFFF);
    auto dt = event.time - m_time;
    if(dt >= 0xFF)
    {
        if(m_end - m_ptr < 2)
            return false;
        *m_ptr = 0xFF000000 | event.time;
        m_ptr++;
        *m_ptr = 0;
    }
    else
    {
        if(m_end - m_ptr < 1)
            return false;
        *m_ptr = dt << 24;
    }
    *m_ptr |= event.bits & 0x00FFFFFF;
    m_ptr++;
    m_time += dt;
    return true;
}

}//namespace r64fx
