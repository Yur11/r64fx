#ifndef R64FX_PLAYER_HPP
#define R64FX_PLAYER_HPP

#include "PlayerView.hpp"

namespace r64fx{

class Player : private PlayerViewFeedbackIface{
    PlayerView* m_view = nullptr;

public:
    Player(Widget* parent_view = nullptr);

    virtual ~Player();

private:
    virtual bool loadAudioFile(const std::string &path);
};

}//namespace r64fx

#endif//R64FX_PLAYER_HPP