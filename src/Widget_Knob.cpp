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
    Image           m_image;

    KnobAnimation(KnobStyle style, int size, int frame_count)
    : m_style(style)
    , m_size(size)
    , m_frame_count(frame_count)
    , m_image(size, size, 2)
    {
        genKnob(&m_image, {0, 0});
    }

    void genKnob(Image* dst, Point<int> dstpos)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill({dst, {dstpos.x(), dstpos.y(), m_size, m_size}}, Color(63, 255));

        Image alpha_mask(m_size, m_size, 1);
        fill(&alpha_mask, 0, 1, 255);

        genDecorationSolidHorseShoe(&alpha_mask);

        genKnobCenter(dst, &alpha_mask, dstpos);
        copy(dst, dstpos, &alpha_mask, ChanShuf(1, 1, 0, 1));
    }

    void genKnobCenter(Image* dst, Image* alpha_mask, Point<int> dstpos)
    {
        {
            Image c0(m_size, m_size, 1);
            fill(&c0, Color(255));
            fill_circle(&c0, 0, 1, Color(0), {5, 5}, m_size - 10);

            copy(alpha_mask, {0, 0}, &c0, PixOpMin());

            Image layer(m_size, m_size, 2);
            fill(&layer, Color(0, 255));
            copy(&layer, {0, 0}, &c0, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }

        {
            Image c1(m_size, m_size, 1);
            fill(&c1, Color(255));
            fill_circle(&c1, 0, 1, Color(0), {6, 6}, m_size - 12);

            Image layer(m_size, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 127, 31, {0, 6, m_size, m_size - 6});
            copy(&layer, {0, 0}, &c1, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }

        {
            Image c2(m_size, m_size, 1);
            fill(&c2, Color(255));
            fill_circle(&c2, 0, 1, Color(0), {8, 8}, m_size - 16);

            Image layer(m_size, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 223, 31, {0, 8, m_size, m_size - 8});
            copy(&layer, {0, 0}, &c2, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }

        {
            Image c3(m_size, m_size, 1);
            fill(&c3, Color(255));
            fill_circle(&c3, 0, 1, Color(0), {9, 9}, m_size - 18);

            Image layer(m_size, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 147, 107, {0, 9, m_size, m_size - 9});
            copy(&layer, {0, 0}, &c3, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }
    }

    void genBaseHorseShoe(Image* horse_shoe)
    {
        genBaseRing(horse_shoe);
        fill_bottom_triangle(
            horse_shoe, 0, 1, Color(255), {0, 0}, m_size
        );
    }

    void genDecorationSolidHorseShoe(Image* alpha_mask)
    {
        Image horse_shoe(m_size, m_size, 1);
        genBaseHorseShoe(&horse_shoe);

//         Image tick(m_size, m_size, 1);
//         fill(&tick, 0, 1, 255);
//         fill({&tick, {m_size/2 - 1, 2, 3, 5}}, 0, 1, 0);
//         rotateAndCopy(&horse_shoe, &tick, -M_PI * 0.75f, PixOpMin());
// 
        mirror_left2right(&horse_shoe);
        copy(alpha_mask, {0, 0}, &horse_shoe, PixOpMin());
/*
        fill({alpha_mask, {halfSize() - 2, 0, 4, 6}}, 0, 1, 255);
        fill({alpha_mask, {halfSize() - 1, 0, 2, 5}}, 0, 1, 0);*/
    }

    void rotateAndCopy(Image* dst, Image* src, float angle, PixelOperation pixop)
    {
        Transformation2D<float> t;
        t.translate(+halfSize() - 0.5f, +halfSize() - 0.5f);
        t.rotate(angle);
        t.translate(-halfSize() + 0.5f, -halfSize() + 0.5f);
        copy(dst, t, src, pixop);
    }

    void genBaseRing(Image* ring)
    {
        fill(
            ring, 0, 1, 255
        );
        fill_circle(
            ring, 0, 1, Color(0), {0, 0}, m_size
        );
        fill_circle(
            ring, 0, 1, Color(255), {3, 3}, m_size - 6
        );
    }

    inline int halfSize() const { return m_size >> 1; }

public:
    KnobStyle  style()       const { return m_style; }
    int        size()        const { return m_size; }
    int        frameCount()  const { return m_frame_count; }

    void paint(Painter* painter)
    {
//         painter->fillRect({0, 0, m_size, m_size}, Color(0, 0, 0, 0));
        painter->putImage(&m_image, {0, 0});
    }

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
    setSize({size, size});
    m_animation = KnobAnimation::getAnimation(style, size);
    onValueChanged(nullptr);
}


Widget_Knob::~Widget_Knob()
{
    if(m_animation)
        KnobAnimation::freeAnimation(m_animation);
}


void Widget_Knob::setValue(float value, bool notify)
{

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


void Widget_Knob::paintEvent(WidgetPaintEvent* event)
{
    cout << "Widget_Knob::paintEvent()\n";
    m_animation->paint(event->painter());
}

// void Widget_Knob::paintAnimation(Painter* painter, int frame_num)
// {
//     static unsigned char bg[4] = {127, 127, 127, 0};
//     static unsigned char c1[4] = {0, 0, 0, 0};
//     static unsigned char c2[4] = {200, 200, 200, 0};
//     static unsigned char* colors[2] = {c1, c2};
// 
//     painter->fillRect({0, 0, width(), height()}, bg);
// 
//     Image frame;
// //     m_animation->pickFrame(&frame, frame_num);
//     painter->blendColors({0, 0}, colors, &frame);
// }


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
