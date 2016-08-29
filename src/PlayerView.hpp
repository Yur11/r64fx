#ifndef R64FX_PLAYER_VIEW_HPP
#define R64FX_PLAYER_VIEW_HPP

#include "Widget.hpp"
#include "Timer.hpp"
#include "Image.hpp"

namespace r64fx{

class PlayerViewControllerIface;
class Widget_Button;
class Widget_BipolarKnob;
class Widget_Slider;

class PlayerViewPrivate;

class PlayerView : public Widget{
    PlayerViewPrivate* m = nullptr;

public:
    PlayerView(PlayerViewControllerIface* feedback, Widget* parent = nullptr);

    virtual ~PlayerView();

    void notifyLoad(bool success);

    void movePlayhead(float seconds);

protected:
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

private:
    void updateCaption(const std::string &caption);

    void updateTempo(float percent);
};


class PlayerViewControllerIface{
public:
    virtual int frameCount() = 0;

    virtual int componentCount() = 0;

    virtual float sampleRate() = 0;

    virtual bool loadAudioFile(const std::string &path) = 0;

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out) = 0;

    virtual void changePitch(float pitch) = 0;

    virtual void changeGain(float gain) = 0;

    virtual void movePlayhead(float seconds) = 0;

    virtual bool hasData() = 0;

    virtual void close() = 0;
};

}//namespace r64fx

#endif//R64FX_PLAYER_VIEW_HPP