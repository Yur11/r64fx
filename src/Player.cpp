#include "Player.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

Player::Player(Widget* parent_view)
{
    auto player_view = new PlayerView(this);
    if(parent_view)
    {
        player_view->setParent(parent_view);
    }
    else
    {
        player_view->show();
    }
    m_view = player_view;
}


Player::~Player()
{
    if(m_view)
    {
        if(m_view->isWindow())
        {
            m_view->close();
        }
        else
        {
            m_view->setParent(nullptr);
        }
        delete m_view;
    }
}


bool Player::loadAudioFile(const std::string &path)
{
    cout << "load: " << path << "\n";
    return false;
}

}//namespace r64fx