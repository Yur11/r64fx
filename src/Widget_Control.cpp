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


ControlAnimation_Value::ControlAnimation_Value(int char_count, Font* font)
: m_font(font)
{
    if(!font)
        return;

    auto glyph = m_font->fetchGlyph("0");
    if(!glyph)
        return;

    setSize({char_count * glyph->width() + 4, m_font->height() + 4});
}


ControlAnimation_Value::~ControlAnimation_Value()
{

}


void ControlAnimation_Value::paint(ControlAnimationState state, Painter* painter)
{
    unsigned char bg[4] = {200, 200, 200, 0};
    unsigned char fg[4] = {0, 0, 0, 0};
    string text = "1234.01\n";
    painter->fillRect({0, 0, width(), height()}, bg);
    if(m_font)
    {
        Image img(width() - 2, height() - 2, 1);
        text2image(text, TextWrap::None, m_font, &img);
        painter->blendColors({2, 2}, Colors(fg), &img);
    }
}


ControlAnimationState ControlAnimation_Value::mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta)
{
    return state;
}


float ControlAnimation_Value::value(ControlAnimationState state, float minval, float maxval)
{
    return 0.0f;
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

    unsigned char color1[2] = {0, 255};
    unsigned char color2[2] = {255, 0};

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
                &img, color1, {cx, cy}, radius - 2,
                normalize_angle(rotation),
                normalize_angle(rotation + full_arc * percent),
                thickness
            );
        }

        if(frame < (frame_count - 1))
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


ControlAnimationState ControlAnimation_Knob::mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta)
{
    int frame = int(state.bits & 0x7F) - delta.y();
    if(frame < 0)
        frame = 0;
    else if(frame > 127)
        frame = 127;
    return ControlAnimationState(frame);
}


float ControlAnimation_Knob::value(ControlAnimationState state, float minval, float maxval)
{
    float range = (maxval - minval);
    float step = range / 127.0f;
    return float(state.bits) * step;
}


ControlAnimation_PlayPauseButton::ControlAnimation_PlayPauseButton(int size)
{
    setSize({size, size});

    int frame_size = width() * height() * 4;
    int frame_count = 4;
    m_frames = new(std::nothrow) unsigned char[frame_size * frame_count];
    if(!m_frames)
        return;

    unsigned char black[4] = {0, 0, 0, 0};
    unsigned char c0[4] = {127, 127, 127, 0};

    unsigned char bg_depressed [4] = {200, 200, 200, 0};
    unsigned char bg_pressed   [4] = {150, 150, 150, 0};
    unsigned char fg_depressed [4] = {100, 100, 100, 0};
    unsigned char fg_pressed   [4] = { 50,  50,  50, 0};

    Image bg_mask(width(), height(), 1);
    {
        fill(&bg_mask, Color(0));
        fill_rounded_rect(&bg_mask, Color(255), {0, 0, width(), height()}, 4);
    }

    Image inset_mask_depressed(width(), height(), 1);
    {
        fill(&inset_mask_depressed, Color(0));
        fill_rounded_rect(&inset_mask_depressed, Color(255), {1, 1, width() - 2, height() - 2}, 4);
    }

    Image inset_mask_pressed(width(), height(), 1);
    {
        fill(&inset_mask_pressed, Color(0));
        fill_rounded_rect(&inset_mask_pressed, Color(255), {2, 2, width() - 4, height() - 4}, 4);
    }

    Image triangle(width()/2 + 1, height()/2 + 1, 1);
    draw_triangles(width()/2 + 1, nullptr, nullptr, nullptr, &triangle);

    Image bars(width()/2 + 1 , height()/2 + 1, 1);
    {
        fill(&bars, Color(0));
        int w = bars.width() / 3;
        int h = bars.height();
        fill(&bars, 0, 255, {0,   0, w, h});
        fill(&bars, 0, 0,   {w,   0, w, h});
        fill(&bars, 0, 255, {w*2, 0, w, h});
    }

    /* 0 Play Depressed */
    {
        Image img(width(), height(), 4, m_frames);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &inset_mask_depressed);
        blend(&img, Point<int>(width()/4 - width()/20, height()/4), Colors(fg_depressed), &triangle);
    }

    /* 1 Play Pressed */
    {
        Image img(width(), height(), 4, m_frames + frame_size);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &inset_mask_pressed);
        blend(&img, Point<int>(width()/4 - width()/20, height()/4), Colors(fg_pressed), &triangle);
    }

    /* 2 Pause Depressed */
    {
        Image img(width(), height(), 4, m_frames + frame_size*2);
        fill(&img, Color(127, 127, 127, 127));
        blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &inset_mask_depressed);
        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), Colors(fg_depressed), &bars);
    }

    /* 3 Pause Pressed */
    {
        Image img(width(), height(), 4, m_frames + frame_size*3);
        fill(&img, Color(127, 127, 127, 127));
        blend(&img, Point<int>(0, 0), Colors(black), &bg_mask);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &inset_mask_pressed);
        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), Colors(fg_pressed), &bars);
    }
}


ControlAnimation_PlayPauseButton::~ControlAnimation_PlayPauseButton()
{
    if(m_frames)
    {
        delete m_frames;
        m_frames = nullptr;
    }
}


void ControlAnimation_PlayPauseButton::paint(ControlAnimationState state, Painter* painter)
{
    int frame_size = width() * height() * 4;
    int frame = state.bits;
    if(m_frames)
    {
        Image img(width(), height(), 4, m_frames + (frame_size * frame));
        painter->putImage(&img, {0, 0});
    }
}


ControlAnimationState ControlAnimation_PlayPauseButton::mousePress(ControlAnimationState state, Point<int> position)
{
    state.bits |= 1;
    return state;
}


ControlAnimationState ControlAnimation_PlayPauseButton::mouseRelease(ControlAnimationState state, Point<int> position)
{
    state.bits = (state.bits ^ 2) & 2;
    return state;
}


float ControlAnimation_PlayPauseButton::value(ControlAnimationState state, float minval, float maxval)
{
    return (state.bits & 2) ? maxval : minval;
}


Widget_Control::Widget_Control(ControlAnimation* animation, Widget* parent)
: Widget(parent)
, m_animation(animation)
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
        stateChanged();
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
        stateChanged();
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
            stateChanged();
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
        stateChanged();
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
        stateChanged();
    }
}


void Widget_Control::stateChanged()
{
    float val = m_animation->value(m_state, 0.0f, 1.0f);
    cout << ">> " << val << "\n";
    static char buff[128];
    int string_size = 0;
    string_size = sprintf(buff, "%f", val);
    if(string_size > 0)
    {
        cout << ((char*) buff) << "\n";
    }
}


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


void Widget_ValueControl::paintEvent(PaintEvent* event)
{
    auto painter = event->painter();
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
        float diff = -0.005 * float(event->delta().y());
        float old_val = value();
        setValue(old_val + diff);
        if(value() != old_val)
        {
            repaint();
        }
    }
}

}//namespace r64fx