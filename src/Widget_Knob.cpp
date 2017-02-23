#include "Widget_Knob.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "ImageUtils.hpp"
#include "WidgetFlags.hpp"
#include "LinkedList.hpp"
#include <cmath>

#include <iostream>
using namespace std;

namespace r64fx{

Font* g_knob_font = nullptr;

class KnobAnimation : public LinkedList<KnobAnimation>::Node{
    Size<int> m_size = {0, 0};
    int m_frame_count = 0;
    unsigned char* m_data = nullptr;
    bool m_is_bipolar = false;

public:
    unsigned long user_count = 0;

    Size<int> size() const
    {
        return m_size;
    }

    int width() const
    {
        return m_size.width();
    }

    int height() const
    {
        return m_size.height();
    }

    int frameCount() const
    {
        return m_frame_count;
    }

    void allocFrames(Size<int> size, int frame_count)
    {
        freeFrames();

        int data_size = frame_count * size.width() * size.height() * 2;
        m_data = new(std::nothrow) unsigned char[data_size];
        m_frame_count = frame_count;
        m_size = size;
    }

    void freeFrames()
    {
        if(m_data)
        {
            delete[] m_data;
            m_data = nullptr;
            m_frame_count = 0;
        }
    }

    void pickFrame(Image* img, int frame)
    {
        img->load(
            width(), height(),
            2, m_data + (frame * width() * height() * 2),
            false
        );
    }


    void genUnipolar()
    {
        unsigned char color1[2] = {255, 0};
        unsigned char color2[2] = {0, 255};

        float cx = width() >> 1;
        float cy = height() >> 1;

        float thickness = 2;
        float radius = (width() >> 1);

        float rotation = M_PI * 0.75f;
        float full_arc = M_PI * 1.5f;
        float frame_count_rcp = 1.0f / float(frameCount() - 1);

        Image circle_mask_img(width(), height(), 1);
        fill_circle(&circle_mask_img, Color(255), Point<float>(cx, cy), radius - 1);
        invert_image(&circle_mask_img, &circle_mask_img);

        Image radius_img(width(), height(), 1);

        for(int frame=0; frame<frameCount(); frame++)
        {
            float percent = float(frame) * frame_count_rcp;

            Image img;
            pickFrame(&img, frame);
            {
                unsigned char color[2] = {0, 0};
                fill(&img, color);
            }

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
                    if(frame == 0)
                        colors[0] = color1;
                    else
                        colors[0] = color2;
                    blend(
                        &img, Point<int>(0, 0), colors, &radius_img
                    );
                }
            }
        }

        m_is_bipolar = false;
    }


    void genBipolar()
    {
        unsigned char color1[2] = {255, 0};
        unsigned char color2[2] = {0, 255};

        float cx = width() >> 1;
        float cy = height() >> 1;

        float thickness = 2;
        float radius = (width() >> 1);

        float rotation = M_PI * 0.75f;
        float full_arc = M_PI * 1.5f;
        float frame_count_rcp = 1.0f / float(frameCount() - 1);

        Image circle_mask_img(width(), height(), 1);
        fill_circle(&circle_mask_img, Color(255), Point<float>(cx, cy), radius - 1);
        invert_image(&circle_mask_img, &circle_mask_img);

        Image radius_img(width(), height(), 1);

        for(int frame=0; frame<frameCount(); frame++)
        {
            float percent = float(frame) * frame_count_rcp;

            Image img;
            pickFrame(&img, frame);
            {
                unsigned char color[2] = {0, 0};
                fill(&img, color);
            }

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

        m_is_bipolar = true;
    }


    bool isBipolar() const
    {
        return m_is_bipolar;
    }
};


namespace{
    LinkedList<KnobAnimation> g_animations;

    KnobAnimation* get_animation(Size<int> size, bool bipolar)
    {
        KnobAnimation* out_anim = nullptr;
        for(auto anim : g_animations)
        {
            if(anim->size() == size && anim->isBipolar() == bipolar)
            {
                out_anim = anim;
                break;
            }
        }

        if(!out_anim)
        {
            out_anim = new KnobAnimation;
            out_anim->allocFrames(size, bipolar ? 127 : 128);
            if(bipolar)
                out_anim->genBipolar();
            else
                out_anim->genUnipolar();
            g_animations.append(out_anim);
        }

        return out_anim;
    }


    void free_animation(KnobAnimation* anim)
    {
        g_animations.remove(anim);
        delete anim;
    }


    void init()
    {
        if(!g_knob_font)
            g_knob_font = new Font("", 14, 72);
    }


    void cleanup()
    {
        if(g_knob_font)
        {
            delete g_knob_font;
            g_knob_font = nullptr;
        }
    }


    unsigned long g_knob_count = 0;


    void on_value_changed_stub(void* arg, Widget_Knob* knob, float new_value)
    {
        knob->setText(new_value);
    }
}//namespace


Widget_Knob::Widget_Knob(Widget* parent)
: Widget(parent)
{
    if(g_knob_count == 0)
    {
        init();
    }
    g_knob_count++;

    onValueChanged(nullptr);
}


Widget_Knob::~Widget_Knob()
{
    if(m_animation)
    {
        m_animation->user_count--;
        if(m_animation->user_count == 0)
        {
            free_animation(m_animation);
            m_animation = nullptr;
        }
    }

    g_knob_count--;
    if(g_knob_count == 0)
    {
        cleanup();
    }
}


float Widget_Knob::value() const
{
    return m_value;
}


void Widget_Knob::setMinValue(float value)
{
    m_min_value = value;
}


float Widget_Knob::minValue() const
{
    return m_min_value;
}


void Widget_Knob::setMaxValue(float value)
{
    m_max_value = value;
}


float Widget_Knob::maxValue() const
{
    return m_max_value;
}


void Widget_Knob::setValueStep(float step)
{
    m_value_step = step;
}


float Widget_Knob::valueStep() const
{
    return m_value_step;
}


float Widget_Knob::valueRange() const
{
    return maxValue() - minValue();
}


bool Widget_Knob::showsText(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_SHOWS_TEXT;
    else
        m_flags &= ~R64FX_WIDGET_SHOWS_TEXT;
    return yes;
}


bool Widget_Knob::showsText() const
{
    return m_flags & R64FX_WIDGET_SHOWS_TEXT;
}


void Widget_Knob::setText(const std::string &text)
{
    m_text = text;
}


void Widget_Knob::setText(float value, std::string prefix, std::string suffix)
{
    char buff[32];
    int nchars = sprintf(buff, "%f", value);
    if(nchars > 0)
    {
        string str(buff);
        str.pop_back();
        str.pop_back();
        str.pop_back();
        str = prefix + str + suffix;
        setText(str);
    }
}


std::string Widget_Knob::text() const
{
    return m_text;
}


void Widget_Knob::onValueChanged(void (*on_value_changed)(void* arg, Widget_Knob* knob, float new_value), void* arg)
{
    if(on_value_changed)
    {
        m_on_value_changed = on_value_changed;
    }
    else
    {
        m_on_value_changed = on_value_changed_stub;
    }
    m_on_value_changed_arg = arg;
}


void Widget_Knob::resizeAndRealign()
{
    if(m_animation)
    {
        setWidth(m_animation->width());
        setHeight(m_animation->height() + (showsText() ? g_knob_font->height() + 2 : 0));
    }


}


void Widget_Knob::setAnimation(KnobAnimation* animation)
{
    m_animation = animation;
    m_animation->user_count++;
}


void Widget_Knob::paintAnimation(Painter* painter, int frame_num)
{
    static unsigned char bg[4] = {127, 127, 127, 0};
    static unsigned char c1[4] = {0, 0, 0, 0};
    static unsigned char c2[4] = {200, 200, 200, 0};
    static unsigned char* colors[2] = {c1, c2};

    painter->fillRect({0, 0, width(), height()}, bg);

    Image frame;
    m_animation->pickFrame(&frame, frame_num);
    painter->blendColors({0, 0}, colors, &frame);
}


void Widget_Knob::paintText(Painter* painter)
{
    static unsigned char fg[4] = {0, 0, 0, 0};
    static unsigned char* colors[1] = {fg};

    Image text_mask;
    text2image(m_text, TextWrap::None, g_knob_font, &text_mask);
    painter->blendColors({width()/2 - text_mask.width()/2, m_animation->height()}, colors, &text_mask);
}


void Widget_Knob::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouseFocus();
    }
}


void Widget_Knob::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left() && isMouseFocusOwner())
        releaseMouseFocus();
}


void Widget_Knob::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        setValue(value() + valueStep() * -event->delta().y(), true);
        repaint();
    }
}


Widget_UnipolarKnob::Widget_UnipolarKnob(Widget* parent)
: Widget_Knob(parent)
{
    setAnimation(get_animation({50, 50}, false));
    setMinValue(0.0f);
    setMaxValue(1.0f);
    setValueStep(0.005f);
    setValue(0.0f, true);
    showsText(true);
    resizeAndRealign();
}


void Widget_UnipolarKnob::setValue(float value, bool notify)
{
    m_value = value;
    if(m_value < m_min_value)
        m_value = m_min_value;
    else if(m_value > m_max_value)
        m_value = m_max_value;
    if(notify)
        m_on_value_changed(m_on_value_changed_arg, this, m_value);
}


bool Widget_UnipolarKnob::isBipolar()
{
    return false;
}


void Widget_UnipolarKnob::paintEvent(WidgetPaintEvent* event)
{
    if(m_animation)
    {
        int frame_num = ((value() - minValue()) / valueRange()) * (m_animation->frameCount() - 1);
        paintAnimation(event->painter(), frame_num);
    }

    if(showsText() && g_knob_font && !text().empty())
    {
        paintText(event->painter());
    }
}



Widget_BipolarKnob::Widget_BipolarKnob(Widget* parent)
: Widget_Knob(parent)
{
    setAnimation(get_animation({50, 50}, true));
    setMinValue(-1.0f);
    setMidValue(0.0f);
    setMaxValue(1.0f);
    setValueStep(0.01);
    setValue(0.0f, true);
    showsText(true);
    resizeAndRealign();
}


void Widget_BipolarKnob::setValue(float value, bool notify)
{
    m_value = value;
    if(abs(m_value - m_mid_value) < m_value_step)
        m_value = m_mid_value;
    else if(m_value < m_min_value)
        m_value = m_min_value;
    else if(m_value > m_max_value)
        m_value = m_max_value;
    if(notify)
        m_on_value_changed(m_on_value_changed_arg, this, m_value);
}


void Widget_BipolarKnob::setMidValue(float value)
{
    m_mid_value = value;
}


float Widget_BipolarKnob::midValue() const
{
    return m_mid_value;
}


float Widget_BipolarKnob::lowerRange() const
{
    return midValue() - minValue();
}


float Widget_BipolarKnob::upperRange() const
{
    return maxValue() - midValue();
}


bool Widget_BipolarKnob::isBipolar()
{
    return true;
}


void Widget_BipolarKnob::paintEvent(WidgetPaintEvent* event)
{
    if(m_animation)
    {
        int frame_num = 0;
        if(value() == midValue())
        {
            frame_num = m_animation->frameCount()/2;
        }
        else if(value() < midValue())
        {
            frame_num = ((value() - minValue()) / lowerRange()) * (m_animation->frameCount()/2 - 1);
        }
        else
        {
            frame_num = ((value() - midValue()) / upperRange()) * (m_animation->frameCount()/2 - 1) + m_animation->frameCount()/2 + 1;
        }
        paintAnimation(event->painter(), frame_num);
    }

    if(showsText() && g_knob_font && !text().empty())
    {
        paintText(event->painter());
    }
}


void Widget_BipolarKnob::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Middle())
    {
        setValue(midValue(), true);
        repaint();
    }
    else
    {
        Widget_Knob::mousePressEvent(event);
    }
}


}//namespace r64fx
