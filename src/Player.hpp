#ifndef R64FX_PLAYER_HPP
#define R64FX_PLAYER_HPP

#include "HostModel.hpp"
#include "PlayerModel.hpp"
#include "PlayerView.hpp"
#include "SoundFile.hpp"

namespace r64fx{

class Player : private PlayerViewIface{
    PlayerModel* m_model = nullptr;
    PlayerView*  m_view = nullptr;
    SoundFile    m_sf;
    std::string  m_path = "";
    float*       m_data = nullptr;

public:
    Player(HostModel* host_model, Widget* parent_view = nullptr);

    virtual ~Player();

    void showView();

private:
    virtual int frameCount();

    virtual int componentCount();

    virtual bool hasData();

    virtual bool loadAudioFile(const std::string &path);

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out);
};

}//namespace r64fx

#endif//R64FX_PLAYER_HPP