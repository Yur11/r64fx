#ifndef R64FX_MIDI_HPP
#define R64FX_MIDI_HPP

namespace r64fx{

class MidiMessage{
    unsigned char m_bytes[4] = {0, 0, 0, 0};

public:
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

    MidiMessage() {}

    MidiMessage(unsigned char* bytes, unsigned char nbytes);

    MidiMessage::Type type() const;

    int channel() const;

    int noteNumber() const;

    int velocity() const;

    int polyaftPressure() const;

    int controllerNumber() const;

    int controllerValue() const;

    int programNuber() const;

    int channelPressure() const;

    int pitchBend() const;

    int songPosition() const;
};


class MidiEvent{
    MidiMessage m_message;
    int m_time = 0;

public:
    MidiEvent() {}

    MidiEvent(const MidiMessage &message, int time)
    : m_message(message)
    , m_time(time)
    {

    }

    inline const MidiMessage &message() const { return m_message; }

    inline int time() const { return m_time; }
};

}//namespace r64fx

#endif//R64FX_MIDI_HPP