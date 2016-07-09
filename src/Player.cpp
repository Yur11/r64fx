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

    m_sf.close();

    if(m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }
}


bool Player::loadAudioFile(const std::string &path)
{
    if(path.empty())
        return false;

    if(!m_view)
        return false;

    m_sf.open(path, SoundFile::Mode::Read);
    if(!m_sf.isGood())
        return false;

    m_path = path;
    m_view->notifySpecs(m_path, m_sf.sampleRate(), m_sf.componentCount(), m_sf.frameCount());

    if(m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }

    int data_size = m_sf.componentCount() * m_sf.frameCount();
    m_data = new(std::nothrow) float[data_size];
    if(!m_data)
    {
        cerr << "Failed to allocate memory!\n";
        return false;
    }

    if(m_sf.readFrames(m_data, m_sf.frameCount()) != m_sf.frameCount())
    {
        cerr << "Failed to read file!\n";
        return false;
    }

    return true;
}

}//namespace r64fx