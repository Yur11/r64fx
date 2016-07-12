#include "Widget_Control.hpp"
#include "Mouse.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "TextPainter.hpp"

#include <cmath>
#include <iostream>
using namespace std;

namespace r64fx{

ControlAnimationState::ControlAnimationState() {}


ControlAnimationState::ControlAnimationState(unsigned long bits) : bits(bits) {}


bool operator==(ControlAnimationState a, ControlAnimationState b)
{
    return a.bits == b.bits;
}


bool operator!=(ControlAnimationState a, ControlAnimationState b)
{
    return !operator==(a, b);
}


void ControlAnimation::setSize(Size<int> size)
{
    m_size = size;
}


Size<int> ControlAnimation::size() const
{
    return m_size;
}


int ControlAnimation::width() const
{
    return m_size.width();
}


int ControlAnimation::height() const
{
    return m_size.height();
}


void ControlAnimation::paint(ControlAnimationState state, Painter* painter)
{

}


ControlAnimationState ControlAnimation::mousePress(ControlAnimationState state, Point<int> position)
{
    return state;
}


ControlAnimationState ControlAnimation::mouseRelease(ControlAnimationState state, Point<int> position)
{
    return state;
}


ControlAnimationState ControlAnimation::mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta)
{
    return state;
}


ControlAnimationState ControlAnimation::mouseEnter(ControlAnimationState state)
{
    return state;
}


ControlAnimationState ControlAnimation::mouseLeave(ControlAnimationState state)
{
    return state;
}


namespace{
    void on_value_changed_stub(Widget_Control*, void*) {}
}


ControlAnimation_Knob::ControlAnimation_Knob(int knob_radius)
{
    setSize({knob_radius, knob_radius});

    int frame_size = width() * height();
    int frame_count = 128;
    int data_size = frame_size * frame_count * 2;
    m_data = new(std::nothrow) unsigned char[data_size];
    if(!m_data)
        return;

    float cx = width() >> 1;
    float cy = height() >> 1;

    float thickness = knob_radius / 20;
    float radius = (width() >> 1);

    float rotation = M_PI * 0.75f;
    float full_arc = M_PI * 1.5f;
    float frame_count_rcp = 1.0f / float(frame_count);

    Image circle_mask_img(width(), height(), 1);
    draw_circle(&circle_mask_img, Color(255), Point<int>(cx, cy), radius - 1);
    invert_image(&circle_mask_img, &circle_mask_img);

    Image radius_img(width(), height(), 1);

    for(int frame=0; frame<frame_count; frame++)
    {
        float percent = float(frame) * frame_count_rcp;

        Image img(width(), height(), 2, m_data + (frame * width() * height() * 2));

        if(frame > 0)
        {
            draw_arc(
                &img, Color(0, 255), {cx, cy}, radius - 2,
                normalize_angle(rotation),
                normalize_angle(rotation + full_arc * percent),
                thickness
            );
        }

        if(frame < (frame_count - 1))
        {
            draw_arc(
                &img, Color(255, 0), {cx, cy}, radius  - 2,
                normalize_angle(rotation + full_arc * percent),
                normalize_angle(rotation + full_arc),
                thickness
            );
        }

        {
            fill(&radius_img, Color(0));
            draw_radius(
                &radius_img, Color(255), {cx, cy},
                normalize_angle(rotation + full_arc * percent),
                (width() * 2) - 1, 0, thickness + 1
            );
            subtract_image(&radius_img, {0, 0}, &circle_mask_img);
            {
                unsigned char* colors[1] = {Color(0, 255)};
                if(frame > 0)
                    colors[0] = Color(0, 255);
                else
                    colors[0] = Color(255, 0);
                blend(
                    &img, Point<int>(0, 0), colors, &radius_img
                );
            }
        }
    }
}


ControlAnimation_Knob::~ControlAnimation_Knob()
{
    if(m_data)
    {
        delete[] m_data;
    }
}


void ControlAnimation_Knob::paint(ControlAnimationState state, Painter* painter)
{
    unsigned char bg[4] = {127, 127, 127, 0};
    painter->fillRect({0, 0, width(), height()}, bg);

    if(m_data)
    {
        int frame = (state.bits & 0x7F);
        Image img(width(), height(), 2, m_data + (frame * width() * height() * 2));

        unsigned char fg1[4] = {0, 0, 0, 0};
        unsigned char fg2[4] = {200, 200, 200, 0};
        unsigned char* colors[2] = {fg1, fg2};
        painter->blendColors({0, 0}, colors, &img);
    }
}


ControlAnimationState ControlAnimation_Knob::mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta)
{
    int frame = int(state.bits & 0x7F) - delta.y();
    if(frame < 0)
        frame = 0;
    else if(frame > 127)
        frame = 127;
    return ControlAnimationState(frame);
}


ControlAnimation_Button::~ControlAnimation_Button()
{
    clear();
}


void ControlAnimation_Button::loadFrames(Size<int> size, unsigned char* frame_data, int frame_count, bool own_data)
{
    setSize(size);
    m_frames = frame_data;
    m_frame_count = frame_count;
    m_owns_frames = own_data;
}


void ControlAnimation_Button::loadColors(Image* template_img, unsigned char** colors, int color_count)
{
    clear();

#ifdef R64FX_DEBUG
    assert(template_img->componentCount() == 1);
#endif//R64FX_DEBUG

    setSize({template_img->width(), template_img->height()});

    int frame_size = width() * height() * 4;
    m_frames = new(std::nothrow) unsigned char[frame_size * color_count];
    if(!m_frames)
        return;

    unsigned char bg[4] = {0, 0, 0, 0};

    for(int c=0; c<color_count; c++)
    {
        Image img(width(), height(), 4, m_frames + (frame_size * c));
        fill(&img, bg);
        blend(&img, Point<int>(0, 0), colors+c, template_img);
    }

    m_frame_count = color_count;
    m_owns_frames = true;
}


void ControlAnimation_Button::clear()
{
    if(m_frames && m_frame_count)
    {
        if(m_owns_frames)
        {
            delete m_frames;
        }
        m_frames = nullptr;
        m_frame_count = 0;
        m_owns_frames = false;
    }
}


void ControlAnimation_Button::paint(ControlAnimationState state, Painter* painter)
{
    int frame_size = width() * height() * 4;
    int frame = state.bits;
    if(m_frames && frame < m_frame_count && frame >= 0)
    {
        Image img(width(), height(), 4, m_frames + (frame_size * frame));
        painter->putImage(&img, {0, 0});
    }
}


ControlAnimationState ControlAnimation_Button::mousePress(ControlAnimationState state, Point<int> position)
{
    int frame = state.bits;
    frame++;
    if(frame >= m_frame_count)
    {
        frame = 0;
    }
    return ControlAnimationState(frame);
}


ControlAnimation_PlayPauseButton::ControlAnimation_PlayPauseButton()
{
    Size<int> size(48, 48);
    setSize(size);

    int frame_size = width() * height() * 4;
    unsigned char* frames = new(std::nothrow) unsigned char[frame_size * 2];
    if(!frames)
        return;

    unsigned char bb[4] = {0, 0, 0, 0};
    unsigned char c0[4] = {127, 127, 127, 0};
    unsigned char bg[4] = {200, 200, 200, 0};
    unsigned char fg[4] = {100, 100, 100, 0};
    unsigned char* bb_colors[1] = {bb};
    unsigned char* bg_colors[1] = {bg};
    unsigned char* fg_colors[1] = {fg};

    Image bg_mask(width(), height(), 1);
    {
        fill(&bg_mask, Color(0));
        fill_rounded_rect(&bg_mask, Color(255), {0, 0, width(), height()}, 4);
    }

    Image inset_mask(width(), height(), 1);
    {
        fill(&inset_mask, Color(0));
        fill_rounded_rect(&inset_mask, Color(255), {1, 1, width() - 2, height() - 2}, 4);
    }

    {
        Image img(width(), height(), 4, frames);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), bb_colors, &bg_mask);
        blend(&img, Point<int>(0, 0), bg_colors, &inset_mask);

        Image triangle(25, 25, 1);
        draw_triangles(25, nullptr, nullptr, nullptr, &triangle);

        blend(&img, Point<int>(10, 12), fg_colors, &triangle);
    }

    {
        Image img(width(), height(), 4, frames + frame_size);
        fill(&img, Color(127, 127, 127, 127));
        blend(&img, Point<int>(0, 0), bb_colors, &bg_mask);
        blend(&img, Point<int>(0, 0), bg_colors, &inset_mask);

        Image bars(25, 25, 1);
        fill(&bars, Color(0));
        int w = bars.width() / 3;
        int h = bars.height();
        fill(&bars, 0, 255, {0,   0, w, h});
        fill(&bars, 0, 0,   {w,   0, w, h});
        fill(&bars, 0, 255, {w*2, 0, w, h});

        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), fg_colors, &bars);
    }

    loadFrames(size, frames, 2, true);
}


Widget_Control::Widget_Control(ControlAnimation* animation, Widget* parent)
: Widget(parent)
, m_animation(animation)
, m_on_value_changed(on_value_changed_stub)
{
    setSize(m_animation->size());
}


Widget_Control::~Widget_Control()
{
    if(m_animation)
    {
        delete m_animation;
    }
}


void Widget_Control::setValue(float value)
{
    if(!m_animation)
        return;

//     auto anim = (ControlAnimation*) m_animation;
//     m_position = anim->boundPosition(anim->valueToPosition(value));
}


float Widget_Control::value() const
{
    if(!m_animation)
        return 0.0f;

//     auto anim = (ControlAnimation*) m_animation;
//     return anim->positionToValue(m_position);
    return 0.0f;
}


void Widget_Control::onValueChanged(void (*callback)(Widget_Control*, void*), void* data)
{
    if(callback)
        m_on_value_changed = callback;
    else
        m_on_value_changed = on_value_changed_stub;
    m_on_value_changed_data = data;
}


void Widget_Control::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    if(m_animation)
    {
        m_animation->paint(m_state, p);
    }
    Widget::paintEvent(event);
}


void Widget_Control::mousePressEvent(MousePressEvent* event)
{
    if(!m_animation)
        return;

    grabMouse();

    auto new_state = m_animation->mousePress(m_state, event->position());
    if(new_state != m_state)
    {
        m_state = new_state;
        repaint();
    }
}


void Widget_Control::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(!m_animation)
        return;

    if(isMouseGrabber())
        ungrabMouse();

    auto new_state = m_animation->mouseRelease(m_state, event->position());
    if(new_state != m_state)
    {
        m_state = new_state;
        repaint();
    }
}


void Widget_Control::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!m_animation)
        return;

    if(event->button() & MouseButton::Left())
    {
        auto new_state = m_animation->mouseMove(m_state, event->position(), event->delta());
        if(new_state != m_state)
        {
            m_state = new_state;
            repaint();
        }
    }
}


void Widget_Control::mouseEnterEvent()
{
    if(!m_animation)
        return;

    auto new_state = m_animation->mouseEnter(m_state);
    if(new_state != m_state)
    {
        m_state = new_state;
        repaint();
    }
}


void Widget_Control::mouseLeaveEvent()
{
    if(!m_animation)
        return;

    auto new_state = m_animation->mouseLeave(m_state);
    if(new_state != m_state)
    {
        m_state = new_state;
        repaint();
    }
}

}//namespace r64fx