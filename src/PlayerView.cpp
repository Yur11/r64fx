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


void PlayerView::notifySpecs(const std::string &path, float samplerate, int channels, int frames)
{
    cout << "File: " << path << "\n";
    cout << "    " << samplerate << ", " << channels << ", " << frames << "\n";

    if(m_waveform)
    {
        delete[] m_waveform;
        m_waveform = nullptr;
    }

    m_waveform = new(std::nothrow) float[channels * frames * 2];
    if(!m_waveform)
        return;

    for(int c=0; c<channels; c++)
    {
        m_player->loadWaveform(0, frames, c, width(), m_waveform + (c * frames));
        cout << "--> " << c << "\n";
    }
    repaint();
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

    if(m_waveform)
    {
        int waveform_height = height() / m_player->componentCount();
        int waveform_y = 0;
        for(int c=0; c<m_player->componentCount(); c++)
        {
            cout << "->> " << waveform_y << "\n";
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