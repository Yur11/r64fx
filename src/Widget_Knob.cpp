#include "Widget_Knob.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include <cmath>

#include <iostream>
using namespace std;

namespace r64fx{

class KnobAnimation{
    Size<int> m_size = {0, 0};
    int m_frame_count = 0;
    unsigned char* m_data = nullptr;

public:
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
        float frame_count_rcp = 1.0f / float(frameCount());

        Image circle_mask_img(width(), height(), 1);
        draw_circle(&circle_mask_img, Color(255), Point<int>(cx, cy), radius - 1);
        invert_image(&circle_mask_img, &circle_mask_img);

        Image radius_img(width(), height(), 1);

        for(int frame=0; frame<frameCount(); frame++)
        {
            float percent = float(frame) * frame_count_rcp;

            Image img;
            pickFrame(&img, frame);

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
        float frame_count_rcp = 1.0f / float(frameCount());

        Image circle_mask_img(width(), height(), 1);
        draw_circle(&circle_mask_img, Color(255), Point<int>(cx, cy), radius - 1);
        invert_image(&circle_mask_img, &circle_mask_img);

        Image radius_img(width(), height(), 1);

        for(int frame=0; frame<frameCount(); frame++)
        {
            float percent = float(frame) * frame_count_rcp;

            Image img;
            pickFrame(&img, frame);

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
    }
};


KnobAnimation* g_unipolar_animation = nullptr;
KnobAnimation* g_bipolar_animation = nullptr;


Widget_Knob::Widget_Knob(Widget* parent)
: Widget(parent)
{

}


float Widget_Knob::value() const
{
    return 0.0f;
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


void Widget_Knob::mousePressEvent(MousePressEvent* event)
{

}


void Widget_Knob::mouseReleaseEvent(MouseReleaseEvent* event)
{

}


void Widget_Knob::mouseMoveEvent(MouseMoveEvent* event)
{

}


Widget_UnipolarKnob::Widget_UnipolarKnob(Widget* parent)
: Widget_Knob(parent)
{
    if(!g_unipolar_animation)
    {
        g_unipolar_animation = new KnobAnimation;
        g_unipolar_animation->allocFrames({50, 50}, 127);
        g_unipolar_animation->genUnipolar();
    }

    setSize(g_unipolar_animation->size());
    m_animation = g_unipolar_animation;
}


void Widget_UnipolarKnob::setValue(float value)
{

}


void Widget_UnipolarKnob::paintEvent(PaintEvent* event)
{
    if(m_animation)
    {
        paintAnimation(event->painter(), m_animation->frameCount() / 3);
    }
}



Widget_BipolarKnob::Widget_BipolarKnob(Widget* parent)
: Widget_Knob(parent)
{
    if(!g_bipolar_animation)
    {
        g_bipolar_animation = new KnobAnimation;
        g_bipolar_animation->allocFrames({50, 50}, 128);
        g_bipolar_animation->genBipolar();
    }

    setSize(g_bipolar_animation->size());
    m_animation = g_bipolar_animation;
}


void Widget_BipolarKnob::setValue(float value)
{

}


void Widget_BipolarKnob::paintEvent(PaintEvent* event)
{
    if(m_animation)
    {
        paintAnimation(event->painter(), m_animation->frameCount() / 3);
    }
}


}//namespace r64fx