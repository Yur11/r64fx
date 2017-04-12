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
    Image           m_marker;
    Image           m_marker_frames;
    Rect<short>     m_marker_frame_coords[32];
    float           m_cut_angle = 0.0f;

    KnobAnimation(KnobStyle style, int size, int frame_count)
    : m_style(style)
    , m_size(size)
    , m_frame_count(frame_count)
    , m_image(size, size, 2)
    , m_marker(size, size, 2)
    {
        genKnob(&m_image, {0, 0});
//         genMarkerFrames();
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

        fill(&m_marker, Color(0, 255));
        fill({&m_marker, {m_size/2 - 2, 7, 4, m_size/2 - 7}}, Color(0, 127));
        fill({&m_marker, {m_size/2 - 1, 8, 2, m_size/2 - 9}}, Color(255, 31));
        for(int i=0; i<4; i++)
        {
            fill({&m_marker, {m_size/2 - 1, 7 + i, 2, 1}}, 1, 1, 191 - 24 * i);
        }
    }

    void genKnobCenter(Image* dst, Image* alpha_mask, Point<int> dstpos)
    {
        {
            Image c0(m_size, m_size, 1);
            fill(&c0, Color(255));
            fill_circle(&c0, 0, 1, Color(0), {5, 5}, m_size - 10);

            copy(alpha_mask, {0, 0}, &c0, ImgCopyMin());

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

    void genBaseHorseShoe(Image* horse_shoe)
    {
        genBaseRing(horse_shoe);
        fill({horse_shoe, {0, m_size-6, m_size/2, 6}}, Color(255));

        for(int x=(m_size/2 - 1); x>1; x--)
        {
            if(horse_shoe->pixel(x, m_size-7)[0] == 0 && horse_shoe->pixel(x-2, m_size-7)[0] != 0)
            {
                fill({horse_shoe, {x+1, m_size-12, m_size/2 - x, 12}}, Color(255));
                m_cut_angle = asin(double(m_size/2 - x) / double(m_size/2));
                break;
            }
        }
    }

    void genDecorationSolidHorseShoe(Image* alpha_mask)
    {
        Image horse_shoe(m_size, m_size, 1);
        genBaseHorseShoe(&horse_shoe);

        mirror_left2right(&horse_shoe);
        copy(alpha_mask, {0, 0}, &horse_shoe, ImgCopyMin());
/*
        fill({alpha_mask, {m_size/2 - 2, 0, 4, 6}}, 0, 1, 255);
        fill({alpha_mask, {m_size/2 - 1, 0, 2, 5}}, 0, 1, 0);*/
    }

    void rotateAndCopy(Image* dst, Image* src, float angle, ImgCopyFlags pixop)
    {
        Transformation2D<float> t;
        t.translate(+(m_size/2) - 0.5f, +(m_size/2) - 0.5f);
        t.rotate(angle);
        t.translate(-(m_size/2) + 0.5f, -(m_size/2) + 0.5f);
        copy(dst, t, src, pixop);
    }

    void genMarker(Image* dst, Point<int> dstpos, float angle)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill(dst, Color(0, 255));

        int hs = m_size / 2;
        Transformation2D<float> t;
        t.translate(float(hs) - 0.5f, float(hs) - 0.5f);
        t.rotate(-angle);
        t.translate(float(-hs) + 0.5f, float(-hs) + 0.5f);
        copy(dst, t, &m_marker, ChanShuf(0, 2, 0, 2));
    }

public:
    KnobStyle  style()       const { return m_style; }
    int        size()        const { return m_size; }
    int        frameCount()  const { return m_frame_count; }

    void paint(Painter* painter, float angle)
    {
        painter->putImage(&m_image, {0, 0});
        Image marker(m_size, m_size, 2);
        genMarker(&marker, {0, 0}, -angle);
        painter->putImage(&marker, {0, 0});
    }

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

    void debugPaint(Painter* painter, Point<int> position)
    {
//         painter->putImage(&m_marker_frames, position);
    }

    float markerAngle(float value, float minval, float maxval)
    {
        float angle = (value - minval) / (maxval - minval);
        angle *= M_PI * (2.0f) - m_cut_angle * 2.0f;
        angle -= M_PI - m_cut_angle;
        return angle;
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
    setMinValue(-1.0f);
    setMaxValue(+1.0f);
    setValue(0.0f);
}


Widget_Knob::~Widget_Knob()
{
    if(m_animation)
        KnobAnimation::freeAnimation(m_animation);
}


void Widget_Knob::setValue(float value, bool notify)
{
    m_value = value;
    if(m_value < minValue())
        m_value = minValue();
    else if(m_value > maxValue())
        m_value = maxValue();
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


void Widget_Knob::debugPaint(Painter* painter, Point<int> position, int size)
{
    auto anim = KnobAnimation::getAnimation(KnobStyle::Bipolar, size);
    anim->debugPaint(painter, position);
    KnobAnimation::freeAnimation(anim);
}


void Widget_Knob::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    m_animation->paint(p, m_animation->markerAngle(m_value, minValue(), maxValue()));
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
