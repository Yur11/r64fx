#ifndef R64FX_PLAYER_VIEW_HPP
#define R64FX_PLAYER_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class PlayerViewEventIface;

class PlayerView : public Widget{
    PlayerViewEventIface* m_event_iface;
    float* m_waveform = nullptr;

public:
    PlayerView(PlayerViewEventIface* feedback, Widget* parent = nullptr);

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
    
    virtual void closeEvent();
};


class PlayerViewEventIface{
public:    
    virtual int frameCount() = 0;

    virtual int componentCount() = 0;

    virtual bool loadAudioFile(const std::string &path) = 0;

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out) = 0;

    virtual bool hasData() = 0;
    
    virtual void close() = 0;
};

}//namespace r64fx

#endif//R64FX_PLAYER_VIEW_HPP