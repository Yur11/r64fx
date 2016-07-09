#include "PlayerView.hpp"
#include "Painter.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "StringUtils.hpp"
#include <string>

#include <iostream>

using namespace std;

namespace r64fx{

PlayerView::PlayerView(PlayerViewFeedbackIface* feedback, Widget* parent)
: m_feedback(feedback)
{
    setSize({800, 240});
}


PlayerView::~PlayerView()
{

}


void PlayerView::notifySpecs(const std::string &path, float samplerate, float channels, float frames)
{
    cout << "File: " << path << "\n";
    cout << "    " << samplerate << ", " << channels << ", " << frames << "\n";
}


void PlayerView::notifyFailedToLoad()
{
    cout << "Failed to load!\n";
}


void PlayerView::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    unsigned char bg[4] = {190, 190, 190, 0};
    p->fillRect({0, 0, width(), height()}, bg);
}


void PlayerView::resizeEvent(ResizeEvent* event)
{
    clip();
    repaint();
}


void PlayerView::mousePressEvent(MousePressEvent* event)
{
    repaint();
}


void PlayerView::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{
    if(event->mode() != ClipboardMode::Clipboard && event->mode() != ClipboardMode::DragAndDrop)
        return;

    if(event->data() == nullptr && event->size() <= 0)
        return;

    if(event->type() == "text/uri-list")
    {
        string uri_list((const char*)event->data(), event->size());
        auto it = uri_list.begin();
        for(;;)
        {
            auto file_path = next_file_path_from_uri_list(it, uri_list.end());
            if(file_path.empty() || m_feedback->loadAudioFile(file_path))
            {
                break;
            }
        }
    }
}


void PlayerView::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{

}


void PlayerView::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{
    if(event->mode() != ClipboardMode::Clipboard)
        return;

    if(event->has("text/uri-list"))
    {
        requestClipboardData("text/uri-list", event->mode());
    }
}


void PlayerView::dndMoveEvent(DndMoveEvent* event)
{
    if(event->has("text/uri-list"))
    {
        event->accept();
    }
    else
    {
        event->reject();
    }
}


void PlayerView::dndDropEvent(DndDropEvent* event)
{
    if(event->has("text/uri-list"))
    {
        event->accept("text/uri-list");
    }
    else
    {
        event->reject();
    }
}


void PlayerView::dndLeaveEvent(DndLeaveEvent* event)
{

}

}//namespace r64fx