#ifndef R64FX_WIDGET_AUDIO_PLAYER_HPP
#define R64FX_WIDGET_AUDIO_PLAYER_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_AudioPlayer : public Widget{
public:
    Widget_AudioPlayer(Widget* parent = nullptr);

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_AUDIO_PLAYER_HPP