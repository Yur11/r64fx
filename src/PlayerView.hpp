#ifndef R64FX_PLAYER_VIEW_HPP
#define R64FX_PLAYER_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class PlayerViewFeedbackIface;

class PlayerView : public Widget{
    PlayerViewFeedbackIface* m_feedback;

public:
    PlayerView(PlayerViewFeedbackIface* feedback, Widget* parent = nullptr);

    virtual ~PlayerView();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);

    virtual void dndMoveEvent(DndMoveEvent* event);

    virtual void dndDropEvent(DndDropEvent* event);

    virtual void dndLeaveEvent(DndLeaveEvent* event);
};


class PlayerViewFeedbackIface{
public:
    virtual bool loadAudioFile(const std::string &path) = 0;
};

}//namespace r64fx

#endif//R64FX_PLAYER_VIEW_HPP