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

    void setPlayheadTime(float seconds);

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
};


class PlayerViewControllerIface{
public:
    virtual int frameCount() = 0;

    virtual int componentCount() = 0;

    virtual float sampleRate() = 0;

    virtual bool loadAudioFile(const std::string &path) = 0;

    virtual bool hasFile() = 0;

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out) = 0;

    virtual void changePitch(float pitch) = 0;

    virtual void changeGain(float gain) = 0;

    virtual void setPlayheadTime(float seconds) = 0;

    virtual float playheadTime() = 0;

    virtual void play() = 0;

    virtual void stop() = 0;

    virtual bool isPlaying() = 0;

    virtual bool hasData() = 0;

    virtual void close() = 0;
};

}//namespace r64fx

#endif//R64FX_PLAYER_VIEW_HPP