#ifndef R64FX_PLAYER_VIEW_HPP
#define R64FX_PLAYER_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class Player;
class PlayerViewIface;

class PlayerView : public Widget{
    friend class Player;

    PlayerViewIface* m_player;
    float* m_waveform = nullptr;

    PlayerView(PlayerViewIface* feedback, Widget* parent = nullptr);

    virtual ~PlayerView();

    void notifyLoad(bool success);

    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);

    virtual void dndMoveEvent(DndMoveEvent* event);

    virtual void dndDropEvent(DndDropEvent* event);

    virtual void dndLeaveEvent(DndLeaveEvent* event);
};


class PlayerViewIface{
public:
    virtual int frameCount() = 0;

    virtual int componentCount() = 0;

    virtual bool loadAudioFile(const std::string &path) = 0;

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out) = 0;

    virtual bool hasData() = 0;
};

}//namespace r64fx

#endif//R64FX_PLAYER_VIEW_HPP