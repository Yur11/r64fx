#include "SignalNode_Player.hpp"

namespace r64fx{

SignalNodeClass_Player::SignalNodeClass_Player(SignalGraph* parent_graph)
: SignalNodeClass(parent_graph)
, m_out         ("out",        0.0f)
, m_playhead    ("playhead",   0.0f)
, m_play_start  ("play_start", 0.0f)
, m_play_end    ("play_end",   0.0f)
, m_pitch       ("pitch",      1.0f)
{

}


SignalNodeClass_Player::~SignalNodeClass_Player()
{

}


SignalNode* SignalNodeClass_Player::newNode(SignalData* data)
{
    auto node = SignalNodeClass::newNode(data->componentCount());
    if(!node)
        return nullptr;

    setNodeData(node, data);

    return node;
}


void SignalNodeClass_Player::forEachPort(void (*fun)(SignalPort* port, void* arg), void* arg)
{
    fun(&m_out,        arg);
    fun(&m_playhead,   arg);
    fun(&m_play_start, arg);
    fun(&m_play_end,   arg);
    fun(&m_pitch,      arg);
}


void SignalNodeClass_Player::process(int sample)
{
    for(int i=0; i<size(); i++)
    {
        float playhead = m_playhead.slot(i);
        float advance = m_pitch.slot(i) * sampleRateReciprocal();
        float range = m_play_end.slot(i) + m_play_start.slot(i);

        playhead += advance;
        if(playhead < m_play_start.slot(i))
        {
            playhead += range;
        }
        else if(playhead > m_play_end.slot(i))
        {
            playhead -= range;
        }

        m_playhead.slot(i) = playhead;
    }

    int i=0;
    for(auto node : m_nodes)
    {
        auto signal_data = (SignalData*) getNodeData(node);
        float sr = float(signal_data->sampleRate());

        for(int s=0; s<node->slotCount(); s++)
        {
            float playhead = m_playhead.slot(i);
            float playhead_frame = playhead * sr;
            int frame_index = int(playhead_frame);

            float value = signal_data->frame(frame_index)[s];

            m_out.slot(i) = value;

            i++;
        }
    }
}

}//namespace r64fx