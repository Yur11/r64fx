#ifndef R64FX_PLAYER_HPP
#define R64FX_PLAYER_HPP

#include "PlayerView.hpp"
#include "SoundFile.hpp"

namespace r64fx{

class Player : private PlayerViewFeedbackIface{
    PlayerView*  m_view = nullptr;
    SoundFile    m_sf;
    std::string  m_path = "";
    float*       m_data = nullptr;

public:
    Player(Widget* parent_view = nullptr);

    virtual ~Player();

private:
    virtual int frameCount();

    virtual int componentCount();

    virtual bool hasData();

    virtual bool loadAudioFile(const std::string &path);

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out);
};

}//namespace r64fx

#endif//R64FX_PLAYER_HPP