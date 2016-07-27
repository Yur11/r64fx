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


void Player::showView()
{
    if(m_view)
    {
        m_view->show();
    }
}


int Player::frameCount()
{
    return m_sf.frameCount();
}


int Player::componentCount()
{
    return m_sf.componentCount();
}


bool Player::hasData()
{
    return m_data != nullptr;
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

    m_path = path;
    m_view->notifyLoad(true);

    return true;
}


void Player::loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out)
{
    if(!m_data)
        return;

    int range_size = end_idx - begin_idx;
    if(range_size < pixel_count)
        return;

    if(component >= m_sf.componentCount())
        return;

    int frames_per_pixel = range_size / pixel_count;

    int f = begin_idx;
    for(auto p=0; p<pixel_count; p++)
    {
        float min = 0.0f;
        float max = 0.0f;

        int i = 0;
        while(i < frames_per_pixel)
        {
            float val = m_data[f + i*m_sf.componentCount() + component];
            if(val < min)
            {
                min = val;
            }
            else if(val > max)
            {
                max = val;
            }
            i++;
        }
        out[p * 2] = min;
        out[p * 2 + 1] = max;

        f += frames_per_pixel;
    }
}

}//namespace r64fx