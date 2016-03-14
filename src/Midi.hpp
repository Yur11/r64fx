#ifndef R64FX_MIDI_HPP
#define R64FX_MIDI_HPP

namespace r64fx{

class MidiMessage{
    unsigned char m_bytes[3] = {0, 0, 0};
    unsigned char m_size = 0;

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

    MidiMessage(unsigned char* bytes, unsigned char nbytes);

    MidiMessage::Type type() const;
};

}//namespace r64fx

#endif//R64FX_MIDI_HPP