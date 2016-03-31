#ifndef R64FX_SIGNAL_NODE_PLAYER_HPP
#define R64FX_SIGNAL_NODE_PLAYER_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_Player : public SignalNode{
    SignalGraph* m_parent_graph = nullptr;
    SignalSource m_out;
    SignalSink   m_playhead;
    SignalSink   m_play_start;
    SignalSink   m_play_end;
    SignalSink   m_pitch;
    int*         m_flags;

public:
    SignalNodeClass_Player(SignalGraph* parent_graph);

    inline SignalSource* out() { return &m_out; }

    inline SignalSink* playhead() { return &m_playhead; }

    inline SignalSink* playStart() { return &m_play_start; }

    inline SignalSink* playEnd() { return &m_play_end; }

    inline SignalSink* pitch() { return &m_pitch; }

    virtual void forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg);

protected:
    virtual void prepare() = 0;

    virtual void process(int sample) = 0;

    virtual void finish() = 0;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_PLAYER_HPP