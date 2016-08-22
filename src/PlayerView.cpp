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
#include <cmath>
#include <iostream>

using namespace std;

namespace r64fx{

Font* g_LargeFont = nullptr;

PlayerView::PlayerView(PlayerViewControllerIface* ctrl, Widget* parent)
: m_ctrl(ctrl)
{
    if(!g_LargeFont)
    {
        g_LargeFont = new Font("", 15, 72);
    }

    m_button_play = new Widget_Button(ButtonAnimation::PlayPause({48, 48}), true, this);

    m_button_cue = new Widget_Button(ButtonAnimation::Text({48, 48}, "CUE", g_LargeFont), true, this);

    m_knob_gain = new Widget_BipolarKnob(this);
    m_knob_gain->setMinValue(-1.0f);
    m_knob_gain->setMidValue(0.0f);
    m_knob_gain->setMaxValue(+1.0f);
    m_knob_gain->setValue(m_knob_gain->midValue());
    m_knob_gain->onValueChanged([](void* arg, Widget_Knob* knob, float new_value){
        auto self = (PlayerView*) arg;
        float gain = pow(2, new_value);
        self->gainChanged(gain);
        knob->setText(gain);
    }, this);

    m_slider_pitch = new Widget_Slider(150, Orientation::Vertical, this);
    m_slider_pitch->setMinValue(-1.0f);
    m_slider_pitch->setMaxValue(+1.0f);
    m_slider_pitch->setValue(0.0f);
    m_slider_pitch->onValueChanged([](void* arg, Widget_Slider*, float slider_pos){
        auto self = (PlayerView*) arg;
        float pitch_shift = -0.5f * slider_pos;
        self->pitchChanged(pitch_shift);
    }, this);
    
    m_timer = new Timer;
    m_timer->onTimeout([](Timer* timer, void* arg){
        timer->stop();
        auto self = (PlayerView*) arg;
        self->pathRecieved();
    }, this);
    
    setSize({800, 240});
}


PlayerView::~PlayerView()
{
    if(m_timer)
        delete m_timer;
}


void PlayerView::notifyLoad(bool success)
{
    if(!success)
    {
        cerr << "Failed to load file!\n";
        return;
    }

    updateCaption(m_path);
    updateWaveform();
    repaint();
}


void PlayerView::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    unsigned char bg[4] = {127, 127, 127, 0};
    p->fillRect({0, 0, width(), height()}, bg);

    int component_count = m_ctrl->componentCount();
    if(m_waveform && component_count > 0)
    {
        if(m_caption_img.isGood())
        {
            static unsigned char fg[4] = {0, 0, 0, 0};
            static unsigned char* colors[1] = {fg};
            
            p->blendColors({0, 0}, colors, &m_caption_img);
        }
        
        int avail_height = height() - g_LargeFont->height();
        
        int waveform_height = avail_height / component_count;
        int waveform_y = 0;
        for(int c=0; c<component_count; c++)
        {
            unsigned char fg[4] = {63, 63, 63, 0};
            p->drawWaveform({60, g_LargeFont->height() + waveform_y, width() - 85, waveform_height}, fg, m_waveform + (c * m_ctrl->frameCount()));
            waveform_y += waveform_height;
        }
    }
    else
    {
        Image textimg;
        text2image("Drop Samples Here", TextWrap::None, g_LargeFont, &textimg);
        unsigned char fg[4] = {0, 0, 0, 0};
        unsigned char* colors[1] = {fg};
        p->blendColors(
            {width()/2 - textimg.width()/2, height()/2 - textimg.height()/2 + g_LargeFont->height()}, colors, &textimg
        );
    }
    
    if(m_tempo_img.isGood())
    {
        static unsigned char fg[4] = {0, 0, 0, 0};
        static unsigned char* colors[1] = {fg};
        
        p->blendColors({width() - m_tempo_img.width(), 0}, colors, &m_tempo_img);
    }
    
    {
        unsigned char fg[4] = {0, 0, 0, 0};
        p->fillRect({0, g_LargeFont->height(), width(), 1}, fg);
    }

    Widget::paintEvent(event);
}


void PlayerView::resizeEvent(ResizeEvent* event)
{
    m_button_play->setPosition({5, height() - 55});
    m_button_cue->setPosition({5, height() - 105});
    m_knob_gain->setPosition({5, height() - 175});
    m_slider_pitch->setPosition({width() - 20, g_LargeFont->height() + 10});
    m_slider_pitch->setHeight(height() - g_LargeFont->height() - 20);
    
    if(m_waveform)
    {
        updateWaveform();
    }
    
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
        {
            auto it = uri_list.begin();
            for(;;)
            {
                if(it == uri_list.end())
                    break;
                
                m_path = next_file_path_from_uri_list(it, uri_list.end());
                if(!m_timer->isRunning())
                {
                    cout << ">>> " << m_path << "\n";
                    m_timer->start();
                }
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


void PlayerView::closeEvent()
{
    m_ctrl->close();
}


void PlayerView::pathRecieved()
{
    cout << "pathRecieved: " << m_path << "\n";
    m_ctrl->loadAudioFile(m_path);
}


void PlayerView::updateCaption(const std::string &caption)
{
    text2image(caption, TextWrap::None, g_LargeFont, &m_caption_img);
}


void PlayerView::pitchChanged(float pitch_shift)
{
    m_ctrl->changePitch(pow(2.0, pitch_shift));
    updateTempo(pitch_shift);
    repaint();
}


void PlayerView::gainChanged(float gain)
{
    m_ctrl->changeGain(gain);
//     updateWaveform();
    repaint();
}


void PlayerView::updateTempo(float percent)
{
    string text = (percent > 0 ? "+" : "") + num2str(percent * 100.0f) + "%";
    text2image(text, TextWrap::None, g_LargeFont, &m_tempo_img);
}


void PlayerView::updateWaveform()
{
    if(m_waveform)
    {
        delete[] m_waveform;
        m_waveform = nullptr;
    }

    int component_count = m_ctrl->componentCount();
    int frame_count = m_ctrl->frameCount();
    
    m_waveform = new(std::nothrow) float[component_count * frame_count * 2];
    if(!m_waveform)
        return;

    for(int c=0; c<component_count; c++)
    {
        m_ctrl->loadWaveform(0, frame_count, c, width(), m_waveform + (c * frame_count));
    }
}

}//namespace r64fx