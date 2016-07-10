#include "PlayerView.hpp"
#include "Painter.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "StringUtils.hpp"
#include <string>

#include <iostream>

using namespace std;

namespace r64fx{

PlayerView::PlayerView(PlayerViewFeedbackIface* player, Widget* parent)
: m_player(player)
{
    setSize({800, 240});
}


PlayerView::~PlayerView()
{

}


void PlayerView::notifyLoad(bool success)
{
    if(!success)
    {
        cerr << "Failed to load file!\n";
        return;
    }

    if(m_waveform)
    {
        delete[] m_waveform;
        m_waveform = nullptr;
    }

    int component_count = m_player->componentCount();
    int frame_count = m_player->frameCount();

    m_waveform = new(std::nothrow) float[component_count * frame_count * 2];
    if(!m_waveform)
        return;

    for(int c=0; c<component_count; c++)
    {
        m_player->loadWaveform(0, frame_count, c, width(), m_waveform + (c * frame_count));
    }
    repaint();
}


void PlayerView::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    unsigned char bg[4] = {190, 190, 190, 0};
    p->fillRect({0, 0, width(), height()}, bg);

    int component_count = m_player->componentCount();

    if(m_waveform)
    {
        int waveform_height = height() / component_count;
        int waveform_y = 0;
        for(int c=0; c<component_count; c++)
        {
            unsigned char fg[3] = {63, 63, 63};
            p->drawWaveform({0, waveform_y, width(), waveform_height}, fg, m_waveform + (c * m_player->frameCount()));
            waveform_y += waveform_height;
        }
    }
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
            if(file_path.empty() || m_player->loadAudioFile(file_path))
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