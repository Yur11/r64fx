#include "PlayerView.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "Timer.hpp"
#include "ImageUtils.hpp"
#include "Font.hpp"
#include "Widget_Button.hpp"
#include "Widget_Knob.hpp"
#include "Widget_Slider.hpp"
#include "StringUtils.hpp"
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

struct Marker : public LinkedList<Marker>::Node{
    enum class Type{
        None,
        Playhead,
        Start,
        Stop,
        LoopIn,
        LoopOut
    };

    Marker::Type  type      = Marker::Type::None;
    float         time      = 0.0f;
    int           position  = 0;

    Marker(Type type = Type::None) : type(type) {}
};

}//namespace


struct PlayerViewPrivate{
    PlayerViewControllerIface* ctrl   = nullptr;

    PlayerView*    parent             = nullptr;

    WaveformPart*   waveform_part      = nullptr;
    unsigned short* waveform           = nullptr;

    TopPart*       top_part           = nullptr;
    LeftPart*      left_part          = nullptr;
    RightPart*     right_part         = nullptr;

    Widget_Button*       button_play  = nullptr;
    Widget_Button*       button_cue   = nullptr;
    Widget_BipolarKnob*  knob_gain    = nullptr;

    Widget_Slider* slider_pitch       = nullptr;

    Timer*       timer                = nullptr;

    std::string  path                 = "";
    float        gain                 = 1.0f;
    float playhead_time               = 0.0f;
    float file_time                   = 0.0f;
    float file_time_rcp               = 0.0f;

    LinkedList<Marker> markers;


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

        m->waveform = new(std::nothrow) unsigned short[component_count * width() * 2];
        if(!m->waveform)
            return;

        for(int c=0; c<component_count; c++)
        {
            m->ctrl->loadWaveform(0, frame_count, c, width(), m->waveform + (c * width() * 2), m->gain);
        }
    }


    void repaintAll()
    {
        m_flags &= ~R64FX_PLAYER_VIEW_REPAINT_PLAYHEAD;
        repaint();
    }


private:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();

        int component_count = m->ctrl->componentCount();
        if(m->waveform && component_count > 0)
        {
            float pt = m->ctrl->playheadTime();
            int pos      = (width() - 1) * pt               * m->file_time_rcp;
            int old_pos  = (width() - 1) * m->playhead_time * m->file_time_rcp;
            m->playhead_time = pt;

            if(m_flags & R64FX_PLAYER_VIEW_REPAINT_PLAYHEAD)
            {
                if(pos != old_pos)
                {
                    paintWaveform(p, component_count, old_pos, 2);
                    paintPlayhead(p, pos);
                }
            }
            else
            {
                paintWaveform(p, component_count, 0, width());
                paintPlayhead(p, pos);
                m_flags |= R64FX_PLAYER_VIEW_REPAINT_PLAYHEAD;
            }
        }
        else
        {
            paintEmpty(p);
        }
    }


    void paintPlayhead(Painter* p, int pos)
    {
        p->fillRect({pos, 0, 2, height()}, Color(255, 0, 0, 0));
    }


    void paintWaveform(Painter* p, int component_count, int pos, int size)
    {
        int waveform_height = height() / component_count;
        int waveform_y = 0;

        p->fillRect({pos, 0, size, height()}, Color(200, 200, 200, 0));

        for(int c=0; c<component_count; c++)
        {
            unsigned char fg[4] = {63, 63, 63, 0};
            p->drawWaveform(
                {pos, waveform_y, size, waveform_height},
                fg, m->waveform + (c * (width()) * 2) + pos * 2
            );
            waveform_y += waveform_height;
        }
    }


    void paintEmpty(Painter* p)
    {
        Image textimg;
        text2image("Drop Samples Here", TextWrap::None, g_LargeFont, &textimg);

        Point<int> pos(width()/2 - textimg.width()/2, height()/2 - textimg.height()/2 + g_LargeFont->height());
        Size<int> s(textimg.width(), textimg.height());

        p->fillRect({pos, s}, Color(127, 127, 127, 0));
        p->blendColors(pos, Color(0, 0, 0, 0), &textimg);
    }


    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        if(event->widthChanged())
        {
            updateWaveform();
        }
        repaintAll();
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        if(event->button() == MouseButton::Left())
        {
            if(m->ctrl->hasFile())
            {
                setPlayheadPosition(event->x());
            }
        }
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(event->button() == MouseButton::Left())
        {
            if(m->ctrl->hasFile())
            {
                setPlayheadPosition(event->x());
            }
        }
    }

    void setPlayheadPosition(int x)
    {
        float time = (float(x) / float(width() - 1)) * m->file_time;
        m->ctrl->setPlayheadTime(time);
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
    virtual void paintEvent(WidgetPaintEvent* event)
    {

    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {

    }
};


class LeftPart : public Widget{
    PlayerViewPrivate* m = nullptr;

public:
    LeftPart(PlayerViewPrivate* p, Widget* parent = nullptr) : Widget(parent), m(p)
    {
//         m->knob_gain = new Widget_BipolarKnob(this);
//         m->knob_gain->setMinValue(-1.0f);
//         m->knob_gain->setMidValue(0.0f);
//         m->knob_gain->setMaxValue(+1.0f);
//         m->knob_gain->setValue(m->knob_gain->midValue());
//         m->knob_gain->onValueChanged([](void* arg, Widget_Knob* knob, float new_value){
//             auto m = (PlayerViewPrivate*) arg;
//             if(new_value < 0)
//             {
//                 new_value *= 2.7f;
//             }
//             float gain = pow(5, new_value);
//             m->gainChanged(gain);
//         }, m);

        m->button_cue = new Widget_Button(ButtonAnimation::Text({48, 48}, "CUE", g_LargeFont), true, this);
        m->button_cue->onStateChanged([](void* arg, Widget_Button* button, unsigned long state){
            auto m = (PlayerViewPrivate*) arg;
            if(m->button_cue->isPressed() && m->ctrl->hasFile())
            {
                m->ctrl->setPlayheadTime(0.0f);
            }
        }, m);

        m->button_play = new Widget_Button(ButtonAnimation::PlayPause({48, 48}), true, this);
        m->button_play->onStateChanged([](void* arg, Widget_Button* button, unsigned long state){
            auto m = (PlayerViewPrivate*) arg;
            if(!m->button_play->isPressed() && m->ctrl->hasFile())
            {
                if(m->ctrl->isPlaying())
                {
                    m->ctrl->stop();
                    m->button_play->setFrame(ButtonAnimation::PlayFrame());
                }
                else
                {
                    m->ctrl->play();
                    m->button_play->setFrame(ButtonAnimation::PauseFrame());
                }
            }
        }, m);

        setWidth(60);
    }

    virtual ~LeftPart()
    {
        if(m->button_play)
        {
            delete m->button_play;
        }

        if(m->button_cue)
        {
            delete m->button_cue;
        }

//         if(m->knob_gain)
//         {
//             delete m->knob_gain;
//         }
    }

protected:
    void resizeEvent(WidgetResizeEvent* event)
    {
//         int buttons_height = m->button_cue->height() + m->button_play->height();
//         int total_height = buttons_height + m->knob_gain->height() + padding;
//         int avail_height = event->height() - padding * 2;
// 
//         if(total_height < avail_height)
//         {
// //             m->knob_gain->setPosition({padding, 0});
// 
//             m->button_cue->setPosition({padding, event->height() - m->button_cue->height() - m->button_play->height() - 1});
//             m->button_play->setPosition({padding, event->height() - m->button_cue->height()});
// 
//             setWidth(padding + max(m->knob_gain->width(), m->button_cue->width()) + padding, false);
//         }
//         else
//         {
// //             m->knob_gain->setPosition({padding + m->button_cue->width() + 1, height() - padding - m->knob_gain->height()});
// 
//             m->button_cue->setPosition({padding, event->height() - m->button_cue->height() - m->button_play->height() - 1});
//             m->button_play->setPosition({padding, event->height() - m->button_cue->height()});
// 
//             setWidth(padding + 1 + m->button_cue->width() + m->knob_gain->width() + padding, false);
//         }
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
    void resizeEvent(WidgetResizeEvent* event)
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
//         static int i = 0;
//         cout << "timer " << i++ << "\n";

        auto m = (PlayerViewPrivate*) arg;
        if(!m->path.empty())
        {
//             cout << "A\n";
            m->pathRecieved();
            m->path.clear();
        }
        else if(m->ctrl->isPlaying())
        {
//             cout << "B\n";
            m->waveform_part->repaint();
        }
//         cout << "-- " << m->ctrl->isPlaying() << "\n";
    }, m);
    m->timer->setInterval(5000);
    m->timer->start();

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

        if(m->top_part)
        {
            delete m->top_part;
        }

        if(m->left_part)
        {
            delete m->left_part;
        }

        if(m->right_part)
        {
            delete m->right_part;
        }

        if(m->timer)
        {
            m->timer->stop();
            delete m->timer;
        }

        delete m;
    }
}


void PlayerView::setPlayheadTime(float seconds)
{
//     int w = width() - 85;
//     float sr = m->ctrl->sampleRate();
//     float fc = m->ctrl->frameCount();
//     if(sr > 0.0f && fc > 0.0f)
//     {
//         float ph = seconds * sr;
//         ph /= fc;
//         ph *= w;
//         if(ph < 0)
//         {
//             ph = 0;
//         }
//         else if(ph >= w)
//         {
//             ph = w - 1;
//         }
//         m->playhead_position = ph;
//         repaint();
//     }
}


void PlayerView::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    childrenPaintEvent(event);
    p->fillRect({0, g_LargeFont->height(), width(), 2}, Color(0, 0, 0, 0));
}


void PlayerView::resizeEvent(WidgetResizeEvent* event)
{
    m->top_part->setWidth(event->width());

    m->left_part->setPosition({0, m->top_part->height() + padding + 2});
    m->left_part->setHeight(event->height() - m->top_part->height() - padding * 2 - 2);

    m->right_part->setPosition({event->width() - m->right_part->width(), m->top_part->height() + padding + 2});
    m->right_part->setHeight(m->left_part->height() - 2);

    m->waveform_part->setPosition({m->left_part->width(), m->top_part->height() + 2});
    m->waveform_part->setSize({
        event->width() - m->left_part->width() - m->right_part->width(),
        event->height() - m->top_part->height() - 2
    });

    clip();
    repaint();
}


void PlayerView::mousePressEvent(MousePressEvent* event)
{
    childrenMousePressEvent(event);
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


void PlayerViewPrivate::pathRecieved()
{
    cout << "pathRecieved: " << path << "\n";
    if(ctrl->loadAudioFile(path))
    {
        waveform_part->updateWaveform();
        file_time = float(ctrl->frameCount()) / float(ctrl->sampleRate());
        if(file_time > 0)
            file_time_rcp = 1.0f / file_time;
        else
            file_time_rcp = 0.0f;
        parent->repaint();
        waveform_part->repaintAll();
    }
}


void PlayerViewPrivate::gainChanged(float gain)
{
    this->gain = gain;
    ctrl->changeGain(gain);
    waveform_part->updateWaveform();
    waveform_part->repaintAll();
}


void PlayerViewPrivate::pitchChanged(float pitch)
{
    ctrl->changePitch(pow(2.0, pitch));
    waveform_part->repaintAll();
}

}//namespace r64fx
