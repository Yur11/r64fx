#ifndef R64FX_MODEL_HPP
#define R64FX_MODEL_HPP

#include "Beat.hpp"

namespace r64fx{

class Sequencer;
class SequencerTrack;
class SequencerClip;
class SequencerClipSource;


struct Sequencer{
    SequencerTrack* root_track = nullptr;
    Beat bpm;
};


struct SequencerTrack{
    SequencerTrack* next_track = nullptr;
    SequencerTrack* sub_tracks = nullptr;
    SequencerClip* clips = nullptr;
};


struct SequencerClip{
    SequencerClipSource* clip_source;
    Beat start, length;
};


struct SequencerClipSource{

};

}//namespace r64fx

#endif//R64FX_MODEL_HPP
