#ifndef R64FX_WIDGET_AUDIO_PLAYER_HPP
#define R64FX_WIDGET_AUDIO_PLAYER_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_AudioPlayer : public Widget{
    float* m_waveform = nullptr;
    std::string m_file_path = "";

public:
    Widget_AudioPlayer(Widget* parent = nullptr);

    virtual ~Widget_AudioPlayer();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);

    virtual void dndEnterEvent(DndEnterEvent* event);

    virtual void dndLeaveEvent(DndLeaveEvent* event);

    virtual void dndMoveEvent(DndMoveEvent* event);

    virtual void dndDropEvent(DndDropEvent* event);

private:
    void reload();
};

}//namespace r64fx

#endif//R64FX_WIDGET_AUDIO_PLAYER_HPP