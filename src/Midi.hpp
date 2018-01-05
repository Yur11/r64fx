#ifndef R64FX_MIDI_HPP
#define R64FX_MIDI_HPP

#include "Debug.hpp"

namespace r64fx{

struct MidiMessage{
    unsigned int bits = 0;

    MidiMessage() {}

    MidiMessage(unsigned char* bytes, unsigned char nbytes)
    {
        bits = 0;
        for(int i=0; i<nbytes; i++)
            bits |= (int(bytes[i]) << (i<<3));
    }

    MidiMessage(unsigned char b0, unsigned char b1, unsigned char b2)
        : bits(b0 | (b1 << 8) | (b2 << 16)) {}

    MidiMessage(unsigned char b0, unsigned char b1)
        : bits(b0 | (b1 << 8)) {}

    MidiMessage(unsigned int bits) : bits(bits) {}

    inline operator unsigned int() const { return bits; }

    inline int byte(unsigned int i) const
    {
        return (bits >> (i<<3)) & 0xFF;
    }

    inline void setByte(unsigned int i, unsigned char byte)
    {
        bits &= ~(0xFF << (i<<3));
        bits |= (int(byte) << (i<<3));
    }

    enum class Type{
        Bad,
        Undefined,
        NoteOff,
        NoteOn,
        PolyAft,
        ControlChange,
        ProgramChange,
        ChanAft,
        PitchBend,
        AllSoundOff,
        ResetAllCtrls,
        LocalControlOff,
        LocalControlOn,
        AllNotesOff,
        OmniOff,
        OmniOn,
        MonoMode,
        PolyMode,
        SysEx,
        TimeCode,
        SongPosition,
        SongSelect,
        TuneRequest,
        SysExEnd,
        Clock,
        Start,
        Continue,
        Stop,
        ActiveSensing,
        Reset
    };

    MidiMessage::Type type() const;

    int byteCount() const;

    inline int channel() const
        { return byte(0) & 0x0F; }

    inline int noteNumber() const
        { return byte(1); }

    inline int velocity() const
        { return byte(2); }

    inline int polyaftPressure() const
        { return byte(2); }

    inline int controllerNumber() const
        { return byte(1); }

    inline int controllerValue() const
        { return byte(2); }

    inline int programNuber() const
        { return byte(1); }

    inline int channelPressure() const
        { return byte(1); }

    inline int pitchBend() const
        { return byte(1) | (byte(2)<<7); }

    inline int songPosition() const
        { return byte(1) | (byte(2)<<7); }

    inline int song() const
        { return byte(1); }


    inline static MidiMessage NoteOff(int channel, int note, int velocity)
        { return MidiMessage(0x80 | channel, note, velocity); }

    inline static MidiMessage NoteOn(int channel, int note, int velocity)
        { return MidiMessage(0x90 | channel, note, velocity); }

    inline static MidiMessage ControlChange(int channel, int controller_number, int controller_value)
    {
        R64FX_DEBUG_ASSERT(controller_number < 120);
        return MidiMessage(0xB0 | channel, controller_number, controller_value);
    }
};


template<typename StreamT> inline StreamT &operator<<(StreamT &stream, MidiMessage msg)
{
    switch(msg.type())
    {
        case MidiMessage::Type::Bad:            { stream << "Bad";        break; }
        case MidiMessage::Type::Undefined:      { stream << "Undefined";  break; }

        case MidiMessage::Type::NoteOff:        { stream << "NoteOff "
            << msg.channel() << ", " << msg.noteNumber() << ", " << msg.velocity(); break; }

        case MidiMessage::Type::NoteOn:         { stream << "NoteOn  "
            << msg.channel() << ", " << msg.noteNumber() << ", " << msg.velocity(); break; }

        case MidiMessage::Type::PolyAft:        { stream << "PolyAft "
            << msg.channel() << ", " << msg.noteNumber() << ", " << msg.polyaftPressure(); break; }

        case MidiMessage::Type::ControlChange:  { stream << "ControlChange "
            << msg.channel() << ", " << msg.controllerValue() << ", " << msg.controllerValue(); break; }

        case MidiMessage::Type::ProgramChange:  { stream << "ProgramChange "
            << msg.channel() << ", " << msg.programNuber(); break; }

        case MidiMessage::Type::ChanAft:        { stream << "ChanAft "
            << msg.channel() << ", " << msg.channelPressure(); break; }

        case MidiMessage::Type::PitchBend:      { stream << "PitchBend "
            << msg.channel() << ", " << msg.pitchBend(); break; }

        case MidiMessage::Type::AllSoundOff:     { stream << "AllNotesOff";     break; }
        case MidiMessage::Type::ResetAllCtrls:   { stream << "ResetAllCtrls";   break; }
        case MidiMessage::Type::LocalControlOff: { stream << "LocalControlOff"; break; }
        case MidiMessage::Type::LocalControlOn:  { stream << "LocalControlOn";  break; }
        case MidiMessage::Type::OmniOff:         { stream << "OmniOff";         break; }
        case MidiMessage::Type::OmniOn:          { stream << "OmniOn";          break; }
        case MidiMessage::Type::MonoMode:        { stream << "MonoMode";        break; }
        case MidiMessage::Type::PolyMode:        { stream << "PolyMode";        break; }
        case MidiMessage::Type::SysEx:           { stream << "SysEx";           break; }
        case MidiMessage::Type::TimeCode:        { stream << "TimeCode";        break; }
        case MidiMessage::Type::SongPosition:    { stream << "SongPosition " << msg.songPosition(); break; }
        case MidiMessage::Type::SongSelect:      { stream << "SongSelect "   << msg.song(); break; }
        case MidiMessage::Type::TuneRequest:     { stream << "TuneRequest";     break; }
        case MidiMessage::Type::SysExEnd:        { stream << "SysExEnd";        break; }
        case MidiMessage::Type::Clock:           { stream << "Clock";           break; }
        case MidiMessage::Type::Start:           { stream << "Start";           break; }
        case MidiMessage::Type::Continue:        { stream << "Continue";        break; }
        case MidiMessage::Type::Stop:            { stream << "Stop";            break; }
        case MidiMessage::Type::ActiveSensing:   { stream << "ActiveSensing";   break; }
        case MidiMessage::Type::Reset:           { stream << "Reset";           break; }

        default:
            break;
    }
    return stream;
}


struct MidiEvent : public MidiMessage{
    unsigned int time = 0;
    MidiEvent() {}
    MidiEvent(MidiMessage msg, unsigned int time = 0) : MidiMessage(msg), time(time) {}
};

template<typename StreamT> inline StreamT &operator<<(StreamT &stream, MidiEvent event)
{
    stream << event.time << ": " << (MidiMessage)event;
    return stream;
}


class MidiEventBuffer{
    unsigned int* m_begin = nullptr;
    unsigned int* m_ptr   = nullptr;
    unsigned int* m_end   = nullptr;
    unsigned int  m_time  = 0;

public:
    MidiEventBuffer(unsigned int* begin, unsigned int* end, unsigned int time = 0)
    : m_begin(begin), m_ptr(begin), m_end(end), m_time(time) {}

    inline unsigned int* begin() const { return m_begin; }

    inline unsigned int* ptr() const { return m_ptr; }

    inline unsigned int* end() const { return m_end; }

protected:
    bool read(MidiEvent &event);

    bool write(MidiEvent event);

    inline bool write(MidiMessage msg, unsigned int time)
        { return write(MidiEvent(msg, time)); }
};

class MidiEventInputBuffer : public MidiEventBuffer{
public:
    using MidiEventBuffer::MidiEventBuffer;
    inline bool read(MidiEvent &event) { return MidiEventBuffer::read(event); }
};

class MidiEventOutputBuffer : public MidiEventBuffer{
public:
    using MidiEventBuffer::MidiEventBuffer;
    template<typename... T> inline bool write(T...t) { return MidiEventBuffer::write(t...); }
};

}//namespace r64fx

#endif//R64FX_MIDI_HPP
