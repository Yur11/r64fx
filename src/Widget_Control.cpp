#include "Widget_Control.hpp"
#include "Mouse.hpp"
#include "Painter.hpp"
#include "ImageAnimation.hpp"
#include "ImageUtils.hpp"

#include <cmath>
#include <iostream>
using namespace std;


namespace r64fx{

struct ControlAnimationImpl : public ControlAnimation{
    int min_position = 0;
    int max_position = 255;

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

    ControlAnimation_Knob(int size)
    {
        imgainim.resize(size, size, 2, positionRange());
        for(int i=0; i<positionRange(); i++)
        {
            unsigned char a[2] = {255, 0};
            unsigned char b[2] = {0, 255};
            unsigned char o[2] = {0, 0};

            int hs = (size / 2);
            int radius = hs - 1;
            int thickness = 2;

            imgainim.pickFrame(i);
            fill(&imgainim, o);

            float angle = normalize_angle((float(i) / (positionRange() - 1)) * 1.5f * M_PI + 0.75f * M_PI);

            if(i > 0)
                draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, angle, thickness);

            if(i < (positionRange()-1))
                draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, angle, M_PI * 0.25f, thickness);

            draw_radius(
                &imgainim, (i>0 ? b : a),
                {float(hs), float(hs)}, angle, radius, 0, thickness + 1
            );
        }
    }

    virtual void repaint(int position, Painter* painter)
    {
        if(imgainim.isGood())
        {
            imgainim.pickFrame(position - min_position);
//             painter->blendColors(&imgainim);
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
            animation = new(std::nothrow) ControlAnimation_Knob(
                min(size.width(), size.height())
            );
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


Widget_Control::Widget_Control(ControlType type, Size<int> size, Widget* parent) : Widget(parent)
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


void Widget_Control::reconfigureEvent(ReconfigureEvent* event)
{
    static unsigned char color[4] = {127, 127, 127, 0};

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
    }
}

}//namespace r64fx