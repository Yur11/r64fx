#include "Player.hpp"
#include "PlayerView.hpp"

namespace r64fx{

Player::Player(Widget* parent_view)
{
    auto player_view = new PlayerView;
    if(parent_view)
    {
        player_view->setParent(parent_view);
    }
    else
    {
        player_view->show();
    }
}

}//namespace r64fx