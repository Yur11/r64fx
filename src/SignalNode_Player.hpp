#ifndef R64FX_SIGNAL_NODE_PLAYER_HPP
#define R64FX_SIGNAL_NODE_PLAYER_HPP

#include "SignalNode.hpp"
#include "SoundDriver.hpp"

namespace r64fx{

class SignalNodeClass_Player : public SignalNode{
    SoundDriver* m_driver = nullptr;
    SignalSource m_out;

public:
    SignalNodeClass_Player(SoundDriver* driver);

    inline SignalSource* out() { return &m_out; }

protected:
    virtual void prepare() = 0;

    virtual void process(int sample) = 0;

    virtual void finish() = 0;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_PLAYER_HPP