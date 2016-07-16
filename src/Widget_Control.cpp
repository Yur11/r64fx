#include "Widget_Control.hpp"
#include "Mouse.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "TextPainter.hpp"

#include <cmath>
#include <iostream>
using namespace std;

namespace r64fx{

ControlAnimation::ControlAnimation(Size<int> size, int frame_count)
: m_size(size)
, m_frame_count(frame_count)
{

}


ControlAnimation::~ControlAnimation()
{

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


int ControlAnimation::frameCount() const
{
    return m_frame_count;
}


ControlAnimation_RGBA::ControlAnimation_RGBA(Size<int> size, int frame_count)
: ControlAnimation(size, frame_count)
{
    if(width() <= 0 || height() <= 0 || frameCount() <=0)
        return;

    int frame_size = width() * height();
    int data_size = frame_size * frameCount() * 2;
    m_data = new(std::nothrow) unsigned char[data_size];
}


ControlAnimation_RGBA::~ControlAnimation_RGBA()
{
    if(m_data)
    {
        delete[] m_data;
    }
}


unsigned char* ControlAnimation_RGBA::data() const
{
    return m_data;
}


void ControlAnimation_RGBA::paint(int frame, Painter* painter)
{
    unsigned char bg[4] = {127, 127, 127, 0};
    painter->fillRect({0, 0, width(), height()}, bg);

    if(m_data && frame < frameCount())
    {
        Image img(width(), height(), 2, m_data + (frame * width() * height() * 2));
        painter->blendColors(
            {0, 0},
            Colors(
                Color(0, 0, 0, 0),
                Color(200, 200, 200, 0)
            ),
            &img
        );
    }
}


ControlAnimation_Knob::ControlAnimation_Knob(int knob_radius, int frame_count)
: ControlAnimation_RGBA({knob_radius, knob_radius}, frame_count)
{
    if(!data())
        return;

    unsigned char color1[2] = {0, 255};
    unsigned char color2[2] = {255, 0};

    float cx = width() >> 1;
    float cy = height() >> 1;

    float thickness = knob_radius / 20;
    float radius = (width() >> 1);

    float rotation = M_PI * 0.75f;
    float full_arc = M_PI * 1.5f;
    float frame_count_rcp = 1.0f / float(frameCount());

    Image circle_mask_img(width(), height(), 1);
    draw_circle(&circle_mask_img, Color(255), Point<int>(cx, cy), radius - 1);
    invert_image(&circle_mask_img, &circle_mask_img);

    Image radius_img(width(), height(), 1);

    for(int frame=0; frame<frameCount(); frame++)
    {
        float percent = float(frame) * frame_count_rcp;

        Image img(width(), height(), 2, data() + (frame * width() * height() * 2));

        if(frame > 0)
        {
            draw_arc(
                &img, color1, {cx, cy}, radius - 2,
                normalize_angle(rotation),
                normalize_angle(rotation + full_arc * percent),
                thickness
            );
        }

        if(frame < (frameCount() - 1))
        {
            draw_arc(
                &img, color2, {cx, cy}, radius  - 2,
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
                unsigned char* colors[1];
                if(frame > 0)
                    colors[0] = color1;
                else
                    colors[0] = color2;
                blend(
                    &img, Point<int>(0, 0), colors, &radius_img
                );
            }
        }
    }
}


ControlAnimation_Button::ControlAnimation_Button(Size<int> size, int frame_count)
: ControlAnimation_RGBA(size, frame_count)
{

}


// ControlAnimation_PlayPauseButton::ControlAnimation_PlayPauseButton(int size)
// {
//     setSize({size, size});
//
//     int frame_size = width() * height() * 4;
//     int frame_count = 4;
//     m_frames = new(std::nothrow) unsigned char[frame_size * frame_count];
//     if(!m_frames)
//         return;
//
//     unsigned char black[4] = {0, 0, 0, 0};
//     unsigned char c0[4] = {127, 127, 127, 0};
//
//     unsigned char bg_depressed [4] = {200, 200, 200, 0};
//     unsigned char bg_pressed   [4] = {150, 150, 150, 0};
//     unsigned char fg_depressed [4] = {100, 100, 100, 0};
//     unsigned char fg_pressed   [4] = { 50,  50,  50, 0};
//
//     Image bg_mask(width(), height(), 1);
//     {
//         fill(&bg_mask, Color(0));
//         fill_rounded_rect(&bg_mask, Color(255), {0, 0, width(), height()}, 4);
//     }
//
//     Image inset_mask_depressed(width(), height(), 1);
//     {
//         fill(&inset_mask_depressed, Color(0));
//         fill_rounded_rect(&inset_mask_depressed, Color(255), {1, 1, width() - 2, height() - 2}, 4);
//     }
//
//     Image inset_mask_pressed(width(), height(), 1);
//     {
//         fill(&inset_mask_pressed, Color(0));
//         fill_rounded_rect(&inset_mask_pressed, Color(255), {2, 2, width() - 4, height() - 4}, 4);
//     }
//
//     Image triangle(width()/2 + 1, height()/2 + 1, 1);
//     draw_triangles(width()/2 + 1, nullptr, nullptr, nullptr, &triangle);
//
//     Image bars(width()/2 + 1 , height()/2 + 1, 1);
//     {
//         fill(&bars, Color(0));
//         int w = bars.width() / 3;
//         int h = bars.height();
//         fill(&bars, 0, 255, {0,   0, w, h});
//         fill(&bars, 0, 0,   {w,   0, w, h});
//         fill(&bars, 0, 255, {w*2, 0, w, h});
//     }
//
//     /* 0 Play Depressed */
//     {
//         Image img(width(), height(), 4, m_frames);
//         fill(&img, c0);
//         blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
//         blend(&img, Point<int>(0, 0), Colors(bg_depressed), &inset_mask_depressed);
//         blend(&img, Point<int>(width()/4 - width()/20, height()/4), Colors(fg_depressed), &triangle);
//     }
//
//     /* 1 Play Pressed */
//     {
//         Image img(width(), height(), 4, m_frames + frame_size);
//         fill(&img, c0);
//         blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
//         blend(&img, Point<int>(0, 0), Colors(bg_pressed), &inset_mask_pressed);
//         blend(&img, Point<int>(width()/4 - width()/20, height()/4), Colors(fg_pressed), &triangle);
//     }
//
//     /* 2 Pause Depressed */
//     {
//         Image img(width(), height(), 4, m_frames + frame_size*2);
//         fill(&img, Color(127, 127, 127, 127));
//         blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
//         blend(&img, Point<int>(0, 0), Colors(bg_depressed), &inset_mask_depressed);
//         blend(&img, Point<int>(
//             img.width() / 2 - bars.width() / 2,
//             img.height() / 2 - bars.height() / 2
//         ), Colors(fg_depressed), &bars);
//     }
//
//     /* 3 Pause Pressed */
//     {
//         Image img(width(), height(), 4, m_frames + frame_size*3);
//         fill(&img, Color(127, 127, 127, 127));
//         blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
//         blend(&img, Point<int>(0, 0), Colors(bg_pressed), &inset_mask_pressed);
//         blend(&img, Point<int>(
//             img.width() / 2 - bars.width() / 2,
//             img.height() / 2 - bars.height() / 2
//         ), Colors(fg_pressed), &bars);
//     }
// }
//
//
// ControlAnimation_PlayPauseButton::~ControlAnimation_PlayPauseButton()
// {
//     if(m_frames)
//     {
//         delete m_frames;
//         m_frames = nullptr;
//     }
// }
//
//
// void ControlAnimation_PlayPauseButton::paint(ControlAnimationState state, Painter* painter)
// {
//     int frame_size = width() * height() * 4;
//     int frame = state.bits;
//     if(m_frames)
//     {
//         Image img(width(), height(), 4, m_frames + (frame_size * frame));
//         painter->putImage(&img, {0, 0});
//     }
// }
//
//
// ControlAnimationState ControlAnimation_PlayPauseButton::mousePress(ControlAnimationState state, Point<int> position)
// {
//     state.bits |= 1;
//     return state;
// }
//
//
// ControlAnimationState ControlAnimation_PlayPauseButton::mouseRelease(ControlAnimationState state, Point<int> position)
// {
//     state.bits = (state.bits ^ 2) & 2;
//     return state;
// }
//
//
// float ControlAnimation_PlayPauseButton::value(ControlAnimationState state, float minval, float maxval)
// {
//     return (state.bits & 2) ? maxval : minval;
// }
//
//
// int ControlAnimation_PlayPauseButton::frameCount()
// {
//     return 0;
// }



Widget_ValueControl::Widget_ValueControl(int char_count, Font* font, Widget* parent)
: Widget(parent)
, m_font(font)
{
    if(!font)
        return;

    auto glyph = m_font->fetchGlyph("0");
    if(!glyph)
        return;

    setSize({char_count * glyph->width() + 4, m_font->height() + 4});
}


Widget_ValueControl::Widget_ValueControl(ControlAnimation* animation, Widget* parent)
: Widget(parent)
{
    setAnimation(animation);
}


Widget_ValueControl::~Widget_ValueControl()
{

}


void Widget_ValueControl::setMinValue(float val)
{
    m_min_value = val;
    if(m_min_value > m_max_value)
        m_min_value = m_max_value;
    setValue(m_value);
}


float Widget_ValueControl::minValue() const
{
    return m_min_value;
}


void Widget_ValueControl::setMaxValue(float val)
{
    m_max_value = val;
    if(m_max_value < m_min_value)
        m_max_value = m_min_value;
    setValue(m_value);
}


float Widget_ValueControl::maxValue() const
{
    return m_max_value;
}


float Widget_ValueControl::valueRange() const
{
    return m_max_value - m_min_value;
}


void Widget_ValueControl::setValue(float val)
{
    m_value = val;
    if(m_value > m_max_value)
        m_value = m_max_value;
    else if(m_value < m_min_value)
        m_value = m_min_value;
}


float Widget_ValueControl::value() const
{
    return m_value;
}


void Widget_ValueControl::setValueStep(float step)
{
    m_value_step = step;
}


float Widget_ValueControl::valueStep() const
{
    return m_value_step;
}


void Widget_ValueControl::setFont(Font* font)
{
    m_font = font;
}


Font* Widget_ValueControl::font() const
{
    return m_font;
}


void Widget_ValueControl::setAnimation(ControlAnimation* animation)
{
    m_animation = animation;
    if(m_animation)
    {
        setSize(m_animation->size());
    }
}


ControlAnimation* Widget_ValueControl::animation() const
{
    return m_animation;
}


void Widget_ValueControl::paintEvent(PaintEvent* event)
{
    auto painter = event->painter();

    if(m_animation)
    {
        int frame_num = int((m_value / valueRange()) * (m_animation->frameCount() - 1));
        m_animation->paint(frame_num, painter);
    }
    else if(m_font)
    {
        unsigned char bg[4] = {200, 200, 200, 0};
        unsigned char fg[4] = {0, 0, 0, 0};
        painter->fillRect({0, 0, width(), height()}, bg);
        char str[128];
        int nchars = sprintf(str, "%f", value());
        if(m_font && nchars > 0)
        {
            Image img(width() - 2, height() - 2, 1);
            text2image(str, TextWrap::None, m_font, &img);
            painter->blendColors({2, 2}, Colors(fg), &img);
        }
    }
}


void Widget_ValueControl::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouse();
    }
}


void Widget_ValueControl::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left() && isMouseGrabber())
    {
        ungrabMouse();
    }
}


void Widget_ValueControl::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left() && isMouseGrabber())
    {
        float diff = -0.007 * float(event->delta().y());
        float old_val = value();
        setValue(old_val + diff);
        if(value() != old_val)
        {
            repaint();
        }
    }
}

}//namespace r64fx