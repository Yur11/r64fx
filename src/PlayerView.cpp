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
#include "ImageUtils.hpp"
#include <cmath>
#include <iostream>

using namespace std;

namespace r64fx{

namespace{

Font* g_LargeFont = nullptr;

constexpr int padding = 5;

class WaveformPart;
class TopPart;
class LeftPart;
class RightPart;

}//namespace


struct PlayerViewPrivate{
    PlayerViewControllerIface* ctrl   = nullptr;

    PlayerView*    parent             = nullptr;

    WaveformPart*  waveform_part      = nullptr;
    float*         waveform           = nullptr;

    TopPart*       top_part           = nullptr;
    LeftPart*      left_part          = nullptr;
    RightPart*     right_part         = nullptr;

    Widget_Button*       button_play  = nullptr;
    Widget_Button*       button_cue   = nullptr;
    Widget_BipolarKnob*  knob_gain    = nullptr;

    Widget_Slider* slider_pitch       = nullptr;

    Timer*       timer                = nullptr;
    std::string  path                 = "";
    Image        caption_img;
    Image        tempo_img;
    float        gain                 = 1.0f;
    int          playhead_position    = 0;


    void pathRecieved();

    void gainChanged(float gain);

    void pitchChanged(float pitch);
};


namespace{

class WaveformPart : public Widget{
    PlayerViewPrivate* m = nullptr;

public:
    WaveformPart(PlayerViewPrivate* p, Widget* parent = nullptr) : Widget(parent), m(p) {}

    void updateWaveform()
    {
        if(m->waveform)
        {
            delete[] m->waveform;
            m->waveform = nullptr;
        }

        int component_count = m->ctrl->componentCount();
        int frame_count = m->ctrl->frameCount();

        m->waveform = new(std::nothrow) float[component_count * width() * 2];
        if(!m->waveform)
            return;

        for(int c=0; c<component_count; c++)
        {
            m->ctrl->loadWaveform(0, frame_count, c, width(), m->waveform + (c * width() * 2));
        }
    }

protected:
    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();

        int component_count = m->ctrl->componentCount();
        if(m->waveform && component_count > 0)
        {
            int waveform_height = height() / component_count;
            int waveform_y = 0;
            for(int c=0; c<component_count; c++)
            {
                unsigned char fg[4] = {63, 63, 63, 0};
                p->drawWaveform(
                    {0, waveform_y, width(), waveform_height},
                    fg, m->waveform + (c * (width()) * 2), m->gain
                );
                waveform_y += waveform_height;
            }

//             p->fillRect({m_playhead_position + 60, g_LargeFont->height(), 2, avail_height}, Color(255, 0, 0, 0));
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
    }

    virtual void resizeEvent(ResizeEvent* event)
    {
        if(event->widthChanged())
        {
            updateWaveform();
        }
        repaint();
    }
};


class TopPart : public Widget{
    PlayerViewPrivate* m = nullptr;

public:
    TopPart(PlayerViewPrivate* p, Widget* parent = nullptr) : Widget(parent), m(p)
    {
        setHeight(g_LargeFont->height());
    }

protected:
    virtual void paintEvent(PaintEvent* event)
    {

    }

    virtual void resizeEvent(ResizeEvent* event)
    {

    }
};


class LeftPart : public Widget{
    PlayerViewPrivate* m = nullptr;

public:
    LeftPart(PlayerViewPrivate* p, Widget* parent = nullptr) : Widget(parent), m(p)
    {
        m->knob_gain = new Widget_BipolarKnob(this);
        m->knob_gain->setMinValue(-1.0f);
        m->knob_gain->setMidValue(0.0f);
        m->knob_gain->setMaxValue(+1.0f);
        m->knob_gain->setValue(m->knob_gain->midValue());
        m->knob_gain->onValueChanged([](void* arg, Widget_Knob* knob, float new_value){
            auto m = (PlayerViewPrivate*) arg;
            if(new_value < 0)
            {
                new_value *= 2.7f;
            }
            float gain = pow(5, new_value);
            m->gainChanged(gain);
            knob->setText(gain);
        }, m);

        m->button_cue = new Widget_Button(ButtonAnimation::Text({48, 48}, "CUE", g_LargeFont), true, this);

        m->button_play = new Widget_Button(ButtonAnimation::PlayPause({48, 48}), true, this);

        setWidth(60);
    }

    virtual ~LeftPart()
    {
        if(m->knob_gain)
            delete m->button_play;

        if(m->button_cue)
            delete m->button_cue;

        if(m->knob_gain)
            delete m->knob_gain;
    }

protected:
    void resizeEvent(ResizeEvent* event)
    {
        int buttons_height = m->button_cue->height() + m->button_play->height();
        int total_height = buttons_height + m->knob_gain->height() + padding;
        int avail_height = event->height() - padding * 2;

        if(total_height < avail_height)
        {
            m->knob_gain->setPosition({padding, 0});

            m->button_cue->setPosition({padding, event->height() - m->button_cue->height() - m->button_play->height() - 1});
            m->button_play->setPosition({padding, event->height() - m->button_cue->height()});

            setWidth(padding + max(m->knob_gain->width(), m->button_cue->width()) + padding, false);
        }
        else
        {
            m->knob_gain->setPosition({padding + m->button_cue->width() + 1, height() - padding - m->knob_gain->height()});

            m->button_cue->setPosition({padding, event->height() - m->button_cue->height() - m->button_play->height() - 1});
            m->button_play->setPosition({padding, event->height() - m->button_cue->height()});

            setWidth(padding + 1 + m->button_cue->width() + m->knob_gain->width() + padding, false);
        }
    }
};


class RightPart : public Widget{
    PlayerViewPrivate* m = nullptr;

public:
    RightPart(PlayerViewPrivate* p, Widget* parent = nullptr) : Widget(parent), m(p)
    {
        m->slider_pitch = new Widget_Slider(150, Orientation::Vertical, this);
        m->slider_pitch->setMinValue(-1.0f);
        m->slider_pitch->setMaxValue(+1.0f);
        m->slider_pitch->setValue(0.0f);
        m->slider_pitch->onValueChanged([](void* arg, Widget_Slider*, float slider_pos){
            auto m = (PlayerViewPrivate*) arg;
            float pitch_shift = -0.5f * slider_pos;
            m->pitchChanged(pitch_shift);
        }, m);

        setWidth(m->slider_pitch->width() + 5);
    }

protected:
    void resizeEvent(ResizeEvent* event)
    {
        m->slider_pitch->setPosition({0, 5});
        m->slider_pitch->setHeight(event->height() - 10);
    }
};

}//namespace


PlayerView::PlayerView(PlayerViewControllerIface* ctrl, Widget* parent)
{
    dndEnabled(true);

    if(!g_LargeFont)
    {
        g_LargeFont = new Font("", 15, 72);
    }

    m = new PlayerViewPrivate;
    m->ctrl           = ctrl;
    m->parent         = this;
    m->waveform_part  = new WaveformPart  (m, this);
    m->top_part       = new TopPart       (m, this);
    m->left_part      = new LeftPart      (m, this);
    m->right_part     = new RightPart     (m, this);

    m->timer = new Timer;
    m->timer->onTimeout([](Timer* timer, void* arg){
        timer->stop();
        auto m = (PlayerViewPrivate*) arg;
        m->pathRecieved();
    }, m);

    setSize({800, 240});
}


PlayerView::~PlayerView()
{
    if(m)
    {
        if(m->waveform_part)
        {
            delete m->waveform_part;
        }
        delete m;
    }

    if(m->timer)
        delete m->timer;
}


void PlayerView::notifyLoad(bool success)
{
    if(!success)
    {
        cerr << "Failed to load file!\n";
        return;
    }

    updateCaption(m->path);
    m->waveform_part->updateWaveform();
    repaint();
}


void PlayerView::movePlayhead(float seconds)
{
    int w = width() - 85;
    float sr = m->ctrl->sampleRate();
    float fc = m->ctrl->frameCount();
    if(sr > 0.0f && fc > 0.0f)
    {
        float ph = seconds * sr;
        ph /= fc;
        ph *= w;
        if(ph < 0)
        {
            ph = 0;
        }
        else if(ph >= w)
        {
            ph = w - 1;
        }
        m->playhead_position = ph;
        repaint();
    }
}


void PlayerView::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    unsigned char bg[4] = {127, 127, 127, 0};
    p->fillRect({0, 0, width(), height()}, bg);

    Widget::paintEvent(event);

    {
        unsigned char fg[4] = {0, 0, 0, 0};
        p->fillRect({0, g_LargeFont->height(), width(), 2}, fg);
    }
}


void PlayerView::resizeEvent(ResizeEvent* event)
{
    m->top_part->setWidth(event->width());

    m->left_part->setPosition({0, m->top_part->height() + padding});
    m->left_part->setHeight(event->height() - m->top_part->height() - padding * 2);

    m->right_part->setPosition({event->width() - m->right_part->width(), m->top_part->height() + padding});
    m->right_part->setHeight(m->left_part->height());

    m->waveform_part->setPosition({m->left_part->width(), m->top_part->height()});
    m->waveform_part->setSize({
        event->width() - m->left_part->width() - m->right_part->width(),
        event->height() - m->top_part->height()
    });

    clip();
    repaint();
}


void PlayerView::mousePressEvent(MousePressEvent* event)
{
//     if(event->y() >= g_LargeFont->height() && event->x() >= 60 && (event->x() + 25) < width())
//     {
//         int playhead_position = event->x() - 60;
//         {
//             m->playhead_position = playhead_position;
//             float sr = m->ctrl->sampleRate();
//             float fc = m->ctrl->frameCount();
//             if(sr > 0.0f && fc > 0.0f)
//             {
//                 int w = width() - 85;
//                 float ph = float(m->playhead_position) / float(w);
//                 ph *= fc;
//                 ph /= sr;
// //                 m->ctrl->movePlayhead(ph);
//             }
//         }
//     }
//     else
    {
        Widget::mousePressEvent(event);
    }
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

                m->path = next_file_path_from_uri_list(it, uri_list.end());
                if(!m->timer->isRunning())
                {
                    m->timer->start();
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
    m->ctrl->close();
}



void PlayerView::updateCaption(const std::string &caption)
{
    text2image(caption, TextWrap::None, g_LargeFont, &(m->caption_img));
}


void PlayerViewPrivate::pathRecieved()
{
    cout << "pathRecieved: " << path << "\n";
    ctrl->loadAudioFile(path);
}


void PlayerViewPrivate::gainChanged(float gain)
{
    ctrl->changeGain(gain);
    gain = gain;
    parent->repaint();
}


void PlayerViewPrivate::pitchChanged(float pitch)
{
    ctrl->changePitch(pow(2.0, pitch));
//     updateTempo(pitch);
    parent->repaint();
}


void PlayerView::updateTempo(float percent)
{
    string text = (percent > 0 ? "+" : "") + num2str(percent * 100.0f) + "%";
    text2image(text, TextWrap::None, g_LargeFont, &(m->tempo_img));
}

}//namespace r64fx