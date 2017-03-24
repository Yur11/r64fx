#include "Widget_Knob.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "ImageUtils.hpp"
#include "WidgetFlags.hpp"
#include <cmath>

#include <iostream>
using namespace std;

namespace r64fx{

Font* g_knob_font = nullptr;

class KnobAnimation : public LinkedList<KnobAnimation>::Node{
    static LinkedList<KnobAnimation> knob_animations;

    int             m_user_count   = 0;
    KnobStyle       m_style;
    int             m_size         = 0;
    int             m_frame_count  = 0;
    unsigned char*  m_data         = nullptr;

    KnobAnimation(KnobStyle style, int size, int frame_count)
    : m_style(style)
    , m_size(size)
    , m_frame_count(frame_count)
    {
    }

public:
    KnobStyle  style()       const { return m_style; }
    int        size()        const { return m_size; }
    int        frameCount()  const { return m_frame_count; }

//     void allocFrames(Size<int> size, int frame_count)
//     {
//         freeFrames();
// 
//         int data_size = frame_count * size.width() * size.height() * 2;
//         m_data = new(std::nothrow) unsigned char[data_size];
//         m_frame_count = frame_count;
//         m_size = size;
//     }
// 
//     void freeFrames()
//     {
//         if(m_data)
//         {
//             delete[] m_data;
//             m_data = nullptr;
//             m_frame_count = 0;
//         }
//     }
// 
//     void pickFrame(Image* img, int frame)
//     {
//         img->load(
//             width(), height(),
//             2, m_data + (frame * width() * height() * 2),
//             false
//         );
//     }


//     void genUnipolar()
//     {
//         unsigned char color1[2] = {255, 0};
//         unsigned char color2[2] = {0, 255};
// 
//         float cx = width() >> 1;
//         float cy = height() >> 1;
// 
//         float thickness = 2;
//         float radius = (width() >> 1);
// 
//         float rotation = M_PI * 0.75f;
//         float full_arc = M_PI * 1.5f;
//         float frame_count_rcp = 1.0f / float(frameCount() - 1);
// 
//         Image circle_mask_img(width(), height(), 1);
// //         fill_circle(&circle_mask_img, Color(255), Point<int>(cx, cy), radius - 1);
//         invert(&circle_mask_img, &circle_mask_img);
// 
//         Image radius_img(width(), height(), 1);
// 
//         for(int frame=0; frame<frameCount(); frame++)
//         {
//             float percent = float(frame) * frame_count_rcp;
// 
//             Image img;
//             pickFrame(&img, frame);
//             {
//                 unsigned char color[2] = {0, 0};
//                 fill(&img, color);
//             }
// 
//             if(frame > 0)
//             {
//                 draw_arc(
//                     &img, color2, {cx, cy}, radius - 2,
//                     normalize_angle(rotation),
//                     normalize_angle(rotation + full_arc * percent),
//                     thickness
//                 );
//             }
// 
//             if(frame < (frameCount() - 1))
//             {
//                 draw_arc(
//                     &img, color1, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation + full_arc * percent),
//                     normalize_angle(rotation + full_arc),
//                     thickness
//                 );
//             }
// 
//             {
//                 fill(&radius_img, Color(0));
//                 draw_radius(
//                     &radius_img, Color(255), {cx, cy},
//                     normalize_angle(rotation + full_arc * percent),
//                     (width() * 2) - 1, 0, thickness + 1
//                 );
//                 subtract_image(&radius_img, {0, 0}, &circle_mask_img);
//                 {
//                     unsigned char* colors[1];
//                     if(frame == 0)
//                         colors[0] = color1;
//                     else
//                         colors[0] = color2;
//                     blend_colors(
//                         &img, Point<int>(0, 0), colors, &radius_img
//                     );
//                 }
//             }
//         }
// 
//         m_is_bipolar = false;
//     }


//     void genBipolar()
//     {
//         unsigned char color1[2] = {255, 0};
//         unsigned char color2[2] = {0, 255};
// 
//         float cx = width() >> 1;
//         float cy = height() >> 1;
// 
//         float thickness = 2;
//         float radius = (width() >> 1);
// 
//         float rotation = M_PI * 0.75f;
//         float full_arc = M_PI * 1.5f;
//         float frame_count_rcp = 1.0f / float(frameCount() - 1);
// 
//         Image circle_mask_img(width(), height(), 1);
// //         fill_circle(&circle_mask_img, Color(255), Point<int>(cx, cy), radius - 1);
//         invert(&circle_mask_img, &circle_mask_img);
// 
//         Image radius_img(width(), height(), 1);
// 
//         for(int frame=0; frame<frameCount(); frame++)
//         {
//             float percent = float(frame) * frame_count_rcp;
// 
//             Image img;
//             pickFrame(&img, frame);
//             {
//                 unsigned char color[2] = {0, 0};
//                 fill(&img, color);
//             }
// 
//             if(frame < (frameCount()/2))
//             {
//                 draw_arc(
//                     &img, color1, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation),
//                     normalize_angle(rotation + full_arc * percent),
//                     thickness
//                 );
// 
//                 draw_arc(
//                     &img, color2, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation + full_arc * percent),
//                     normalize_angle(rotation + full_arc * 0.5),
//                     thickness
//                 );
// 
//                 draw_arc(
//                     &img, color1, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation + full_arc * 0.5),
//                     normalize_angle(rotation + full_arc),
//                     thickness
//                 );
//             }
//             else if(frame == (frameCount()/2))
//             {
//                 draw_arc(
//                     &img, color1, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation),
//                     normalize_angle(rotation + full_arc),
//                     thickness
//                 );
//             }
//             else
//             {
//                 draw_arc(
//                     &img, color1, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation),
//                     normalize_angle(rotation + full_arc * 0.5),
//                     thickness
//                 );
// 
//                 draw_arc(
//                     &img, color2, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation + full_arc * 0.5),
//                     normalize_angle(rotation + full_arc * percent),
//                     thickness
//                 );
// 
//                 draw_arc(
//                     &img, color1, {cx, cy}, radius  - 2,
//                     normalize_angle(rotation + full_arc * percent),
//                     normalize_angle(rotation + full_arc),
//                     thickness
//                 );
//             }
// 
//             {
//                 fill(&radius_img, Color(0));
//                 draw_radius(
//                     &radius_img, Color(255), {cx, cy},
//                     normalize_angle(rotation + full_arc * percent),
//                     (width() * 2) - 1, 0, thickness + 1
//                 );
//                 subtract_image(&radius_img, {0, 0}, &circle_mask_img);
//                 {
//                     unsigned char* colors[1];
//                     {
//                         if(frame == (frameCount()/2))
//                             colors[0] = color1;
//                         else
//                             colors[0] = color2;
//                     }
//                     blend_colors(
//                         &img, Point<int>(0, 0), colors, &radius_img
//                     );
//                 }
//             }
//         }
// 
//         m_is_bipolar = true;
//     }


//     bool isBipolar() const
//     {
//         return m_is_bipolar;
//     }

    static KnobAnimation* getAnimation(KnobStyle style, int size)
    {
        KnobAnimation* anim = nullptr;
        for(auto ka : knob_animations)
        {
            if(ka->style() == style && ka->size() == size)
            {
                anim = ka;
                break;
            }
        }

        if(!anim)
        {
            anim = new KnobAnimation(style, size, 256);
            knob_animations.append(anim);
        }
        anim->m_user_count++;
        return anim;
    }

    static void freeAnimation(KnobAnimation* anim)
    {
        anim->m_user_count--;
        if(anim->m_user_count == 0)
        {
            knob_animations.remove(anim);
            delete anim;
        }
    }
};

LinkedList<KnobAnimation> KnobAnimation::knob_animations;


namespace{
    void on_value_changed_stub(void* arg, Widget_Knob* knob, float new_value) {}
}//namespace


Widget_Knob::Widget_Knob(KnobStyle style, int size, Widget* parent)
: Widget(parent)
{
    m_animation = KnobAnimation::getAnimation(style, size);
    onValueChanged(nullptr);
}


Widget_Knob::~Widget_Knob()
{
    if(m_animation)
        KnobAnimation::freeAnimation(m_animation);
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


void Widget_Knob::paintAnimation(Painter* painter, int frame_num)
{
    static unsigned char bg[4] = {127, 127, 127, 0};
    static unsigned char c1[4] = {0, 0, 0, 0};
    static unsigned char c2[4] = {200, 200, 200, 0};
    static unsigned char* colors[2] = {c1, c2};

    painter->fillRect({0, 0, width(), height()}, bg);

    Image frame;
//     m_animation->pickFrame(&frame, frame_num);
    painter->blendColors({0, 0}, colors, &frame);
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

}//namespace r64fx
