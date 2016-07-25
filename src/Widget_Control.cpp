#include "Widget_Control.hpp"
#include "WidgetFlags.hpp"
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


ControlAnimation_Image::ControlAnimation_Image(FrameFormat ff, Size<int> size, int frame_count)
: ControlAnimation(size, frame_count)
, m_frame_format(ff)
{
    if(frameSize() <= 0 || frameCount() <=0)
        return;

    int data_size = frameSize() * frameCount();
    m_data = new(std::nothrow) unsigned char[data_size];
}


ControlAnimation_Image::~ControlAnimation_Image()
{
    if(m_data)
    {
        delete[] m_data;
    }
}


ControlAnimation_Image::FrameFormat ControlAnimation_Image::frameFormat() const
{
    return m_frame_format;
}


int ControlAnimation_Image::frameComponentCount() const
{
    switch(m_frame_format)
    {
        case ControlAnimation_Image::FrameFormat::FullRGBA:
            return 4;

        case ControlAnimation_Image::FrameFormat::BlendedRG:
            return 2;

        default:
            return 0;
    }
}


int ControlAnimation_Image::frameSize() const
{
    return width() * height() * frameComponentCount();
}


unsigned char* ControlAnimation_Image::data() const
{
    return m_data;
}


void ControlAnimation_Image::pickFrame(int frame, Image* img)
{
    img->load(width(), height(), frameComponentCount(), m_data + (frameSize() * frame), false);
}


void ControlAnimation_Image::paint(int frame, Painter* painter)
{
    unsigned char bg[4] = {127, 127, 127, 0};
    painter->fillRect({0, 0, width(), height()}, bg);

    if(m_data && frame < frameCount())
    {
        Image img;
        pickFrame(frame, &img);

        if(frameFormat() == ControlAnimation_Image::FrameFormat::FullRGBA)
        {
            painter->putImage(&img, {0, 0});
        }
        else if(frameFormat() == ControlAnimation_Image::FrameFormat::BlendedRG)
        {
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
}


ControlAnimation_Knob::ControlAnimation_Knob(int knob_radius, int frame_count, KnobType knob_type)
: ControlAnimation_Image(
    ControlAnimation_Image::FrameFormat::BlendedRG,
    {knob_radius, knob_radius},
    frame_count + (knob_type == KnobType::Unipolar || (frame_count & 1) ? 0 : 1)
)
, m_type(knob_type)
{
    if(!data())
        return;

    unsigned char color1[2] = {255, 0};
    unsigned char color2[2] = {0, 255};

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

        Image img;
        pickFrame(frame, &img);

        if(knob_type == KnobType::Unipolar)
        {
            if(frame > 0)
            {
                draw_arc(
                    &img, color2, {cx, cy}, radius - 2,
                    normalize_angle(rotation),
                    normalize_angle(rotation + full_arc * percent),
                    thickness
                );
            }

            if(frame < (frameCount() - 1))
            {
                draw_arc(
                    &img, color1, {cx, cy}, radius  - 2,
                    normalize_angle(rotation + full_arc * percent),
                    normalize_angle(rotation + full_arc),
                    thickness
                );
            }
        }
        else if(knob_type == KnobType::Bipolar)
        {
            if(frame < (frameCount()/2))
            {
                draw_arc(
                    &img, color1, {cx, cy}, radius  - 2,
                    normalize_angle(rotation),
                    normalize_angle(rotation + full_arc * percent),
                    thickness
                );

                draw_arc(
                    &img, color2, {cx, cy}, radius  - 2,
                    normalize_angle(rotation + full_arc * percent),
                    normalize_angle(rotation + full_arc * 0.5),
                    thickness
                );

                draw_arc(
                    &img, color1, {cx, cy}, radius  - 2,
                    normalize_angle(rotation + full_arc * 0.5),
                    normalize_angle(rotation + full_arc),
                    thickness
                );
            }
            else if(frame == (frameCount()/2))
            {
                draw_arc(
                    &img, color1, {cx, cy}, radius  - 2,
                    normalize_angle(rotation),
                    normalize_angle(rotation + full_arc),
                    thickness
                );
            }
            else
            {
                draw_arc(
                    &img, color1, {cx, cy}, radius  - 2,
                    normalize_angle(rotation),
                    normalize_angle(rotation + full_arc * 0.5),
                    thickness
                );

                draw_arc(
                    &img, color2, {cx, cy}, radius  - 2,
                    normalize_angle(rotation + full_arc * 0.5),
                    normalize_angle(rotation + full_arc * percent),
                    thickness
                );

                draw_arc(
                    &img, color1, {cx, cy}, radius  - 2,
                    normalize_angle(rotation + full_arc * percent),
                    normalize_angle(rotation + full_arc),
                    thickness
                );
            }
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
                if(knob_type == KnobType::Unipolar)
                {
                    if(frame == 0)
                        colors[0] = color1;
                    else
                        colors[0] = color2;
                }
                else
                {
                    if(frame == (frameCount()/2))
                        colors[0] = color1;
                    else
                        colors[0] = color2;
                }
                blend(
                    &img, Point<int>(0, 0), colors, &radius_img
                );
            }
        }
    }
}


void ControlAnimation_Knob::setType(KnobType type)
{
    m_type = type;
}


KnobType ControlAnimation_Knob::type() const
{
    return m_type;
}


ControlAnimation_Button::ControlAnimation_Button(Size<int> size, int frame_count)
: ControlAnimation_Image(ControlAnimation_Image::FrameFormat::FullRGBA, size, frame_count)
{

}


void ControlAnimation_Button::generateMasks(Image* bg, Image* depressed, Image* pressed)
{
    bg->load(width(), height(), 1);
    {
        fill(bg, Color(0));
        fill_rounded_rect(bg, Color(255), {0, 0, width(), height()}, 4);
    }

    depressed->load(width(), height(), 1);
    {
        fill(depressed, Color(0));
        fill_rounded_rect(depressed, Color(255), {1, 1, width() - 2, height() - 2}, 4);
    }

    pressed->load(width(), height(), 1);
    {
        fill(pressed, Color(0));
        fill_rounded_rect(pressed, Color(255), {2, 2, width() - 4, height() - 4}, 4);
    }
}


ControlAnimation_ColouredButton::ControlAnimation_ColouredButton(int size, unsigned char** rgbas, int num_rgbas)
: ControlAnimation_Button({size, size}, num_rgbas * 2)
{
    if(!data())
        return;

    unsigned char black[4] = {0, 0, 0, 0};

    Image bg, depressed, pressed;
    generateMasks(&bg, &depressed, &pressed);

    for(int i=0; i<frameCount()/2; i++)
    {
        /* Depressed */
        {
            Image img;
            pickFrame(i*2, &img);
            blend(&img, Point<int>(0, 0), Colors(black), &bg);
            blend(&img, Point<int>(0, 0), rgbas + i, &depressed);
        }

        /* Pressed */
        {
            Image img;
            pickFrame(i*2 + 1, &img);
            blend(&img, Point<int>(0, 0), Colors(black), &bg);
            blend(&img, Point<int>(0, 0), rgbas + i, &pressed);
        }
    }
}


int ControlAnimation_ColouredButton::mousePress(int current_frame)
{
    current_frame++;
    if(current_frame >= frameCount())
        current_frame = 0;
    return current_frame;
}


int ControlAnimation_ColouredButton::mouseRelease(int current_frame)
{
    current_frame++;
    if(current_frame >= frameCount())
        current_frame = 0;
    return current_frame;
}


enum PlayPauseStates{
    PlayDepressed   = 0,
    PlayPressed     = 1,
    PauseDepressed  = 2,
    PausePressed    = 3
};


ControlAnimation_PlayPauseButton::ControlAnimation_PlayPauseButton(int size)
: ControlAnimation_Button({size, size}, 4)
{
    if(!data())
        return;

    unsigned char black[4] = {0, 0, 0, 0};
    unsigned char c0[4] = {127, 127, 127, 0};

    unsigned char bg_depressed [4] = {200, 200, 200, 0};
    unsigned char bg_pressed   [4] = {150, 150, 150, 0};
    unsigned char fg_depressed [4] = {100, 100, 100, 0};
    unsigned char fg_pressed   [4] = { 50,  50,  50, 0};

    Image bg, depressed, pressed;
    generateMasks(&bg, &depressed, &pressed);

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
        Image img;
        pickFrame(PlayDepressed, &img);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &depressed);
        blend(&img, Point<int>(width()/4 - width()/20, height()/4), Colors(fg_depressed), &triangle);
    }

    /* 1 Play Pressed */
    {
        Image img;
        pickFrame(PlayPressed, &img);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &pressed);
        blend(&img, Point<int>(width()/4 - width()/20, height()/4), Colors(fg_pressed), &triangle);
    }

    /* 2 Pause Depressed */
    {
        Image img;
        pickFrame(PauseDepressed, &img);
        fill(&img, Color(127, 127, 127, 127));
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &depressed);
        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), Colors(fg_depressed), &bars);
    }

    /* 3 Pause Pressed */
    {
        Image img;
        pickFrame(PausePressed, &img);
        fill(&img, Color(127, 127, 127, 127));
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &pressed);
        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), Colors(fg_pressed), &bars);
    }
}


int ControlAnimation_PlayPauseButton::mousePress(int current_frame)
{
    if(current_frame == PlayDepressed)
        return PlayPressed;
    if(current_frame == PauseDepressed)
        return PausePressed;
    return 0;
}


int ControlAnimation_PlayPauseButton::mouseRelease(int current_frame)
{
    if(current_frame == PlayPressed)
        return PauseDepressed;
    if(current_frame == PausePressed)
        return PlayDepressed;
    return 0;
}



Widget_ValueControl::Widget_ValueControl(Widget* parent)
: Widget(parent)
{

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


bool Widget_ValueControl::showsText(bool yes)
{
//     if(yes)
//         m_flags |= R64FX_WIDGET_CONTROL_SHOWS_TEXT;
//     else
//         m_flags &= ~R64FX_WIDGET_CONTROL_SHOWS_TEXT;
    return yes;
}


bool Widget_ValueControl::showsText() const
{
    return false;
}


void Widget_ValueControl::resizeAndRealign()
{
    int text_width = textWidth();
    int anim_width = animationWidth();
    setSize({max(text_width, anim_width), textHeight() + animationHeight()});
}


int Widget_ValueControl::textWidth() const
{
    if(!showsText())
        return 0;

    if(!m_font)
        return 0;

    auto glyph = m_font->fetchGlyph("0");
    if(!glyph)
        return 0;

    return glyph->width() * 9;
}


int Widget_ValueControl::textHeight() const
{
    if(!m_font)
        return 0;
    return m_font->height();
}


int Widget_ValueControl::animationWidth() const
{
    if(!m_animation)
        return 0;

    return m_animation->width();
}


int Widget_ValueControl::animationHeight() const
{
    if(!m_animation)
        return 0;

    return m_animation->height();
}


void Widget_ValueControl::paintEvent(PaintEvent* event)
{
    auto painter = event->painter();

    if(m_animation)
    {
        paintAnimation(painter, {0, 0});
    }

    if(m_font && showsText())
    {
        int y = 0;
        if(m_animation)
        {
            y = animationHeight();
            paintText(painter, {0, y});
        }
    }
}


void Widget_ValueControl::paintAnimation(Painter* painter, Point<int> position)
{
    auto old_offset = painter->offset();
    painter->setOffset(old_offset + position);

    int frame_num = int(((m_value - minValue()) / valueRange()) * (m_animation->frameCount() - 1));
    m_animation->paint(frame_num, painter);

    painter->setOffset(old_offset);
    cout << frame_num << " -> ";
}


void Widget_ValueControl::paintText(Painter* painter, Point<int> position)
{
    auto old_offset = painter->offset();
    painter->setOffset(old_offset + position);

    unsigned char fg[4] = {0, 0, 0, 0};
    char str[32];
    int nchars = sprintf(str, "%f", value());
    if(m_font && nchars > 0)
    {
        if(nchars > 3)
        {
            str[nchars - 3] = '\0';
        }

        Image* img = text2image(str, TextWrap::None, m_font);

        unsigned char bg[4] = {127, 127, 127, 0};
        painter->fillRect({0, 0, img->width() + 10, img->height()}, bg);

        painter->blendColors({2, 2}, Colors(fg), img);

        cout << str << "\n";
    }

    painter->setOffset(old_offset);
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
        float diff = -valueStep() * float(event->delta().y());
        float old_val = value();
        setValue(old_val + diff);
        if(value() != old_val)
        {
            repaint();
        }
    }
}


Widget_ButtonControl::Widget_ButtonControl(ControlAnimation_Button* animation, Widget* parent)
: Widget(parent)
{
    setAnimation(animation);
}


void Widget_ButtonControl::setAnimation(ControlAnimation_Button* animation)
{
    m_animation = animation;
    if(m_animation)
    {
        setSize(animation->size());
    }
}


void Widget_ButtonControl::paintEvent(PaintEvent* event)
{
    if(m_animation)
    {
        m_animation->paint(m_frame, event->painter());
    }
}


void Widget_ButtonControl::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouse();
        if(m_animation)
        {
            m_frame = m_animation->mousePress(m_frame);
            repaint();
        }
    }
}


void Widget_ButtonControl::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left() && isMouseGrabber())
    {
        ungrabMouse();
        if(m_animation)
        {
            m_frame = m_animation->mouseRelease(m_frame);
            repaint();
        }
    }
}

}//namespace r64fx