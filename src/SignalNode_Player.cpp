#include "SignalNode_Player.hpp"

namespace r64fx{

SignalNodeClass_Player::SignalNodeClass_Player(SignalGraph* parent_graph)
: m_parent_graph(parent_graph)
, m_out         ("out")
, m_playhead    ("playhead")
, m_play_start  ("play_start")
, m_play_end    ("play_end")
, m_pitch       ("pitch")
{

}


void SignalNodeClass_Player::forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg)
{
    fun(&m_out,        arg);
    fun(&m_playhead,   arg);
    fun(&m_play_start, arg);
    fun(&m_play_end,   arg);
    fun(&m_pitch,      arg);
}

}//namespace r64fx