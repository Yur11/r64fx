#include "Widget_Control.hpp"
#include "Mouse.hpp"
#include "Painter.hpp"
#include "ImageAnimation.hpp"
#include "ImageUtils.hpp"

#include <cmath>
#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    void on_value_changed_stub(Widget_Control*, void*) {}
}

struct ControlAnimationImpl : public ControlAnimation{
    int min_position = 0;
    int max_position = 255;
    float min_value = 0.0f;
    float max_value = 1.0f;

    virtual ~ControlAnimationImpl() {}

    int positionRange() const
    {
        return max_position - min_position + 1;
    }

    int newPosition(int old_position, MouseMoveEvent* event)
    {
        int pos = old_position - event->dy();
        if(pos < min_position)
            pos = min_position;
        else if(pos > max_position)
            pos = max_position;
        return pos;
    }

    virtual void repaint(int position, Painter* painter) = 0;

    float positionToValue(int position)
    {
        int position_range = max_position - min_position;
        float value_range = max_value - min_value;
        return float(position) * (value_range / float(position_range));
    }

    int valueToPosition(float value)
    {
        int position_range = max_position - min_position;
        float value_range = max_value - min_value;
        return value * (float(position_range) / value_range);
    }
};


struct ControlAnimationEntry{
    ControlType        type;
    Size<int>          size;
    ControlAnimation*  animation;

    ControlAnimationEntry(ControlType type, Size<int> size, ControlAnimation* animation)
    : type(type)
    , size(size)
    , animation(animation)
    {

    }
};


inline bool operator==(const ControlAnimationEntry &a, const ControlAnimationEntry &b)
{
    return a.type == b.type && a.size == b.size;
}


vector<ControlAnimationEntry> g_animations;


float normalize_angle(float angle)
{
    while(angle > (2.0f * M_PI))
        angle -= (2.0f * M_PI);

    while(angle < 0.0f)
        angle += (2.0f * M_PI);

    return angle;
}


struct ControlAnimation_Knob : public ControlAnimationImpl{
    ImageAnimation imgainim;

    virtual void repaint(int position, Painter* painter)
    {
        unsigned char a[4] = {1, 96, 242, 0};
        unsigned char b[4] = {242, 96, 1, 0};
        unsigned char* colors[2] = {a, b};

        if(imgainim.isGood())
        {
            imgainim.pickFrame(position - min_position);
            painter->blendColors({0, 0}, colors, &imgainim);
        }
    }
};


struct ControlAnimation_Knob_UnipolarLarge : public ControlAnimation_Knob{
    ControlAnimation_Knob_UnipolarLarge(int size)
    {
        unsigned char a[2] = {255, 0};
        unsigned char b[2] = {0, 255};
        unsigned char o[2] = {0, 0};

        int hs = (size / 2);
        int radius = hs - 1;
        int thickness = 2;

        imgainim.resize(size, size, 2, positionRange());
        for(int i=0; i<positionRange(); i++)
        {
            imgainim.pickFrame(i);
            fill(&imgainim, o);

            float angle = normalize_angle((float(i) / (positionRange() - 1)) * 1.5f * M_PI + 0.75f * M_PI);

            if(i > 0)
                draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, angle, thickness);

            if(i < (positionRange()-1))
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle, M_PI * 0.25f, thickness);

            draw_radius(
                &imgainim, (i>0 ? b : a),
                {float(hs), float(hs)}, angle, radius, 0, thickness + 1
            );
        }
    }
};


struct ControlAnimation_Knob_BipolarLarge : public ControlAnimation_Knob{
    ControlAnimation_Knob_BipolarLarge(int size)
    {
        unsigned char a[2] = {255, 0};
        unsigned char b[2] = {0, 255};
        unsigned char o[2] = {0, 0};

        int hs = (size / 2);
        int radius = hs - 1;
        int thickness = 2;

        imgainim.resize(size, size, 2, positionRange());
        for(int i=0; i<positionRange(); i++)
        {
            imgainim.pickFrame(i);
            fill(&imgainim, o);

            float angle = normalize_angle((float(i) / (positionRange() - 1)) * 1.5f * M_PI + 0.75f * M_PI);

            if(i == positionRange()/2)
            {
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, M_PI * 0.25f, thickness);
                draw_radius(
                    &imgainim, a,
                    {float(hs), float(hs)}, angle, radius, 0, thickness + 1
                );
            }
            else
            {
                if(i < positionRange()/2)
                {
                    draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, angle,        thickness);
                    draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, angle,        M_PI * 0.25f, thickness);
                    draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f,  M_PI * 0.25f, thickness);
                }
                else
                {
                    draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, M_PI * 1.5f,  thickness);
                    draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f,  angle,        thickness);
                    draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle,        M_PI * 0.25f, thickness);
                }

                draw_radius(
                    &imgainim, b,
                    {float(hs), float(hs)}, angle, radius, 0, thickness + 1
                );
            }
        }
    }
};


struct ControlAnimation_Knob_UnipolarSector : public ControlAnimation_Knob{
    ControlAnimation_Knob_UnipolarSector(int size)
    {
        imgainim.resize(size, size, 2, positionRange());
        for(int i=0; i<positionRange(); i++)
        {
            unsigned char a[2] = {255, 0};
            unsigned char b[2] = {0, 255};
            unsigned char o[2] = {0, 0};

            int hs = (size / 2);
            int radius = hs - 1;
            int thickness = radius - 1;

            imgainim.pickFrame(i);
            fill(&imgainim, o);

            float angle = normalize_angle((float(i) / (positionRange() - 1)) * 2.0f * M_PI + 0.5f * M_PI);

            if(i == 0)
            {
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, 0.0f, M_PI * 2.0f,  thickness);
            }
            else if(i == (positionRange() - 1))
            {
                draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, 0.0f, M_PI * 2.0f,  thickness);
            }
            else
            {
                draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 0.5f, angle, thickness);
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle, M_PI * 0.5f, thickness);
            }
        }
    }
};


struct ControlAnimation_Knob_BipolarSector : public ControlAnimation_Knob{
    ControlAnimation_Knob_BipolarSector(int size)
    {
        imgainim.resize(size, size, 2, positionRange());
        for(int i=0; i<positionRange(); i++)
        {
            unsigned char a[2] = {255, 0};
            unsigned char b[2] = {0, 255};
            unsigned char o[2] = {0, 0};

            int hs = (size / 2);
            int radius = hs - 1;
            int thickness = radius - 1;

            imgainim.pickFrame(i);
            fill(&imgainim, o);

            float angle = normalize_angle((float(i) / (positionRange() - 1)) * 2.0f * M_PI + 0.5f * M_PI);

            if(i == (positionRange()/2))
            {
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, 0.0f, M_PI * 2.0f,  thickness);
            }
            else if(i < (positionRange()/2))
            {
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.5f, angle,       thickness);
                draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, angle,       M_PI * 1.5f, thickness);
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f, M_PI * 0.5f, thickness);
            }
            else
            {
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.5f, M_PI * 1.5f, thickness);
                draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f, angle,       thickness);
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle,       M_PI * 0.5f, thickness);
            }
        }
    }
};


ControlAnimation* newAnimation(ControlType type, Size<int> size)
{
    for(auto entry : g_animations)
    {
        if(entry == ControlAnimationEntry(type, size, nullptr))
        {
            return entry.animation;
        }
    }

    ControlAnimation* animation = nullptr;
    switch(type)
    {
        case ControlType::UnipolarRadius:
        {
            animation = new(std::nothrow) ControlAnimation_Knob_UnipolarLarge(
                min(size.width(), size.height())
            );
            break;
        }

        case ControlType::BipolarRadius:
        {
            animation = new(std::nothrow) ControlAnimation_Knob_BipolarLarge(
                min(size.width(), size.height())
            );
            break;
        }

        case ControlType::UnipolarSector:
        {
            animation = new(std::nothrow) ControlAnimation_Knob_UnipolarSector(
                min(size.width(), size.height())
            );
            break;
        }

        case ControlType::BipolarSector:
        {
            animation = new(std::nothrow) ControlAnimation_Knob_BipolarSector(
                min(size.width(), size.height())
            );
            break;
        }

        default:
        {
            break;
        }
    }

    if(animation)
    {
        g_animations.push_back({type, size, animation});
    }

    return animation;
}


Widget_Control::Widget_Control(ControlType type, Size<int> size, Widget* parent)
: Widget(parent)
, m_on_value_changed(on_value_changed_stub)
{
    m_animation = newAnimation(type, size);
    setSize(size);
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

    auto anim = (ControlAnimationImpl*) m_animation;
    m_position = anim->valueToPosition(value);
}


float Widget_Control::value() const
{
    if(!m_animation)
        return 0.0f;

    auto anim = (ControlAnimationImpl*) m_animation;
    return anim->positionToValue(m_position);
}


void Widget_Control::onValueChanged(void (*callback)(Widget_Control*, void*), void* data)
{
    if(callback)
        m_on_value_changed = callback;
    else
        m_on_value_changed = on_value_changed_stub;
    m_on_value_changed_data = data;
}


void Widget_Control::reconfigureEvent(ReconfigureEvent* event)
{
    static unsigned char color[4] = {161, 172, 176, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, color);
    if(m_animation)
    {
        auto anim = (ControlAnimationImpl*) m_animation;
        anim->repaint(m_position, p);
    }
    Widget::reconfigureEvent(event);
}


void Widget_Control::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
}


void Widget_Control::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!m_animation)
        return;

    if(event->button() & MouseButton::Left())
    {
        auto anim = (ControlAnimationImpl*) m_animation;
        m_position = anim->newPosition(m_position, event);
        update();
        m_on_value_changed(this, m_on_value_changed_data);
    }
}

}//namespace r64fx