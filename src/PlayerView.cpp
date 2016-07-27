#include "PlayerView.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "StringUtils.hpp"
#include <string>
#include "Font.hpp"
#include "Widget_Button.hpp"
#include "Widget_Knob.hpp"
#include "Widget_Slider.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

Font* g_LargeFont;

PlayerView::PlayerView(PlayerViewFeedbackIface* player, Widget* parent)
: m_player(player)
{
    if(g_LargeFont)
    {
        g_LargeFont = new Font("", 15, 72);
    }

    setSize({800, 240});

    auto button_play = new Widget_Button(ButtonAnimation::PlayPause({48, 48}), true, this);
    button_play->setPosition({5, height() - 55});

    auto button_cue = new Widget_Button(ButtonAnimation::Text({48, 48}, "CUE", g_LargeFont), true, this);
    button_cue->setPosition({5, height() - 105});

    auto knob_gain = new Widget_BipolarKnob(this);
    knob_gain->setPosition({5, height() - 175});
    knob_gain->setMinValue(0.0f);
    knob_gain->setMidValue(1.0f);
    knob_gain->setMaxValue(2.0f);
    knob_gain->setValue(knob_gain->midValue());

    auto slider_pitch = new Widget_Slider(150, Orientation::Vertical, this);
    slider_pitch->setPosition({width() - 20, 10});
    slider_pitch->setHeight(height() - 20);
    slider_pitch->setValue(0.5f);
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
    unsigned char bg[4] = {127, 127, 127, 0};
    p->fillRect({0, 0, width(), height()}, bg);

    int component_count = m_player->componentCount();

    if(m_waveform)
    {
        int waveform_height = height() / component_count;
        int waveform_y = 0;
        for(int c=0; c<component_count; c++)
        {
            unsigned char fg[4] = {63, 63, 63, 0};
            p->drawWaveform({60, waveform_y, width() - 85, waveform_height}, fg, m_waveform + (c * m_player->frameCount()));
            waveform_y += waveform_height;
        }
    }
    else
    {
        Image textimg;
        text2image("Drop samples here!", TextWrap::None, g_LargeFont, &textimg);
        unsigned char fg[4] = {0, 0, 0, 0};
        unsigned char* colors[1] = {fg};
        p->blendColors(
            {width()/2 - textimg.width()/2, height()/2 - textimg.height()/2}, colors, &textimg
        );
    }

    Widget::paintEvent(event);
}


void PlayerView::resizeEvent(ResizeEvent* event)
{
    clip();
    repaint();
}


void PlayerView::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
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