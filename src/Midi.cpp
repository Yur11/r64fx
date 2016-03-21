#include "Midi.hpp"


namespace r64fx{

MidiMessage::MidiMessage(unsigned char* bytes, unsigned char nbytes)
{
    for(int i=0; i<nbytes; i++)
    {
        m_bytes[i] = bytes[i];
    }
}


MidiMessage::Type chan_mode(const unsigned char* bytes)
{
    switch(bytes[1])
    {
        case 120:
            return MidiMessage::Type::AllSoundOff;

        case 121:
            return MidiMessage::Type::ResetAllCtrls;

        case 122:
        {
            if(bytes[1] == 0)
                return MidiMessage::Type::LocalControlOff;
            else if(bytes[1] == 127)
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


MidiMessage::Type system_common(const unsigned char* bytes)
{
    switch(bytes[0])
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
    switch(m_bytes[0] & 0xF0)
    {
        case 0x80:
            return MidiMessage::Type::NoteOff;

        case 0x90:
            return MidiMessage::Type::NoteOn;

        case 0xA0:
            return MidiMessage::Type::PolyAft;

        case 0xB0:
        {
            if(m_bytes[1] < 120)
                return MidiMessage::Type::ControlChange;
            else
                return chan_mode(m_bytes);
        }

        case 0xC0:
            return MidiMessage::Type::ProgramChange;

        case 0xD0:
            return MidiMessage::Type::ChanAft;

        case 0xE0:
            return MidiMessage::Type::PitchBend;

        case 0xF0:
            return system_common(m_bytes);

        default:
            return MidiMessage::Type::Bad;
    }
}


int MidiMessage::channel() const
{
    return (m_bytes[0] & 0x0F);
}


int MidiMessage::noteNumber() const
{
    return m_bytes[1];
}


int MidiMessage::velocity() const
{
    return m_bytes[2];
}


int MidiMessage::polyaftPressure() const
{
    return m_bytes[2];
}


int MidiMessage::controllerNumber() const
{
    return m_bytes[1];
}


int MidiMessage::controllerValue() const
{
    return m_bytes[2];
}


int MidiMessage::programNuber() const
{
    return m_bytes[1];
}


int MidiMessage::channelPressure() const
{
    return m_bytes[1];
}


int MidiMessage::pitchBend() const
{
    int l = m_bytes[1];
    int m = m_bytes[2];
    return l | (m<<7);
}


int MidiMessage::songPosition() const
{
    int l = m_bytes[1];
    int m = m_bytes[2];
    return l | (m<<7);
}

}//namespace r64fx