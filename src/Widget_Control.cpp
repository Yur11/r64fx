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


namespace{
    void on_value_changed_stub(Widget_Control*, void*) {}
}


Widget_Control::Widget_Control(ControlAnimation* animation, Widget* parent)
: Widget(parent)
, m_animation(animation)
, m_on_value_changed(on_value_changed_stub)
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


void Widget_Control::setValue(float value)
{
    if(!m_animation)
        return;

//     auto anim = (ControlAnimation*) m_animation;
//     m_position = anim->boundPosition(anim->valueToPosition(value));
}


float Widget_Control::value() const
{
    if(!m_animation)
        return 0.0f;

//     auto anim = (ControlAnimation*) m_animation;
//     return anim->positionToValue(m_position);
    return 0.0f;
}


void Widget_Control::onValueChanged(void (*callback)(Widget_Control*, void*), void* data)
{
    if(callback)
        m_on_value_changed = callback;
    else
        m_on_value_changed = on_value_changed_stub;
    m_on_value_changed_data = data;
}


void Widget_Control::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();
    if(m_animation)
    {
        m_animation->paint(m_state, p);
    }
    Widget::updateEvent(event);
}


void Widget_Control::mousePressEvent(MousePressEvent* event)
{
    if(!m_animation)
        return;

    auto new_state = m_animation->mousePress(m_state, event->position());
    if(new_state != m_state)
    {
        m_state = new_state;
        update();
    }
}


void Widget_Control::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(!m_animation)
        return;

    auto new_state = m_animation->mouseRelease(m_state, event->position());
    if(new_state != m_state)
    {
        m_state = new_state;
        update();
    }
}


void Widget_Control::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!m_animation)
        return;

    auto new_state = m_animation->mouseMove(m_state, event->position(), event->delta());
    if(new_state != m_state)
    {
        m_state = new_state;
        update();
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
        update();
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
        update();
    }
}


float normalize_angle(float angle)
{
    while(angle > (2.0f * M_PI))
        angle -= (2.0f * M_PI);

    while(angle < 0.0f)
        angle += (2.0f * M_PI);

    return angle;
}


void ControlAnimation_Knob::repaint(int position, Painter* painter)
{
    unsigned char a[4] = {1, 96, 242, 0};
    unsigned char b[4] = {242, 96, 1, 0};
    unsigned char* colors[2] = {a, b};

    if(imgainim.isGood())
    {
//         imgainim.pickFrame(position - minPosition());
        painter->blendColors({0, 0}, colors, &imgainim);
    }
}


ControlAnimation_Knob_UnipolarLarge::ControlAnimation_Knob_UnipolarLarge(int size)
{
//     setMinValue(0.0f);
//     setMaxValue(1.0f);

//     unsigned char a[2] = {255, 0};
//     unsigned char b[2] = {0, 255};
//     unsigned char o[2] = {0, 0};
//
//     int hs = (size / 2);
//     int radius = hs - 1;
//     int thickness = 2;

//     imgainim.resize(size, size, 2, positionRange());
//     for(int i=0; i<positionRange(); i++)
//     {
//         imgainim.pickFrame(i);
//         fill(&imgainim, o);
//
//         float angle = normalize_angle((float(i) / (positionRange() - 1)) * 1.5f * M_PI + 0.75f * M_PI);
//
//         if(i > 0)
//             draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, angle, thickness);
//
//         if(i < (positionRange()-1))
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle, M_PI * 0.25f, thickness);
//
//         draw_radius(
//             &imgainim, (i>0 ? b : a),
//             {float(hs), float(hs)}, angle, radius, 0, thickness + 1
//         );
//     }
}


ControlAnimation_Knob_BipolarLarge::ControlAnimation_Knob_BipolarLarge(int size)
{
//     setMinValue(-1.0f);
//     setMaxValue(+1.0f);

//     unsigned char a[2] = {255, 0};
//     unsigned char b[2] = {0, 255};
//     unsigned char o[2] = {0, 0};
//
//     int hs = (size / 2);
//     int radius = hs - 1;
//     int thickness = 2;
//
//     imgainim.resize(size, size, 2, positionRange());
//     for(int i=0; i<positionRange(); i++)
//     {
//         imgainim.pickFrame(i);
//         fill(&imgainim, o);
//
//         float angle = normalize_angle((float(i) / (positionRange() - 1)) * 1.5f * M_PI + 0.75f * M_PI);
//
//         if(i == positionRange()/2)
//         {
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, M_PI * 0.25f, thickness);
//             draw_radius(
//                 &imgainim, a,
//                 {float(hs), float(hs)}, angle, radius, 0, thickness + 1
//             );
//         }
//         else
//         {
//             if(i < positionRange()/2)
//             {
//                 draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, angle,        thickness);
//                 draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, angle,        M_PI * 0.25f, thickness);
//                 draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f,  M_PI * 0.25f, thickness);
//             }
//             else
//             {
//                 draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.75f, M_PI * 1.5f,  thickness);
//                 draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f,  angle,        thickness);
//                 draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle,        M_PI * 0.25f, thickness);
//             }
//
//             draw_radius(
//                 &imgainim, b,
//                 {float(hs), float(hs)}, angle, radius, 0, thickness + 1
//             );
//         }
//     }
}


ControlAnimation_Knob_UnipolarSector::ControlAnimation_Knob_UnipolarSector(int size)
{
//     setMinValue(0.0f);
//     setMaxValue(1.0f);

//     imgainim.resize(size, size, 2, positionRange());
//     for(int i=0; i<positionRange(); i++)
//     {
//         unsigned char a[2] = {255, 0};
//         unsigned char b[2] = {0, 255};
//         unsigned char o[2] = {0, 0};
//
//         int hs = (size / 2);
//         int radius = hs - 1;
//         int thickness = radius - 1;
//
//         imgainim.pickFrame(i);
//         fill(&imgainim, o);
//
//         float angle = normalize_angle((float(i) / (positionRange() - 1)) * 2.0f * M_PI + 0.5f * M_PI);
//
//         if(i == 0)
//         {
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, 0.0f, M_PI * 2.0f,  thickness);
//         }
//         else if(i == (positionRange() - 1))
//         {
//             draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, 0.0f, M_PI * 2.0f,  thickness);
//         }
//         else
//         {
//             draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 0.5f, angle, thickness);
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle, M_PI * 0.5f, thickness);
//         }
//     }
}


ControlAnimation_Knob_BipolarSector::ControlAnimation_Knob_BipolarSector(int size)
{
//     setMinValue(-1.0f);
//     setMaxValue(+1.0f);

//     imgainim.resize(size, size, 2, positionRange());
//     for(int i=0; i<positionRange(); i++)
//     {
//         unsigned char a[2] = {255, 0};
//         unsigned char b[2] = {0, 255};
//         unsigned char o[2] = {0, 0};
//
//         int hs = (size / 2);
//         int radius = hs - 1;
//         int thickness = radius - 1;
//
//         imgainim.pickFrame(i);
//         fill(&imgainim, o);
//
//         float angle = normalize_angle((float(i) / (positionRange() - 1)) * 2.0f * M_PI + 0.5f * M_PI);
//
//         if(i == (positionRange()/2))
//         {
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, 0.0f, M_PI * 2.0f,  thickness);
//         }
//         else if(i < (positionRange()/2))
//         {
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.5f, angle,       thickness);
//             draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, angle,       M_PI * 1.5f, thickness);
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f, M_PI * 0.5f, thickness);
//         }
//         else
//         {
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, M_PI * 0.5f, M_PI * 1.5f, thickness);
//             draw_arc(&imgainim, b, {float(hs), float(hs)}, radius - 1, M_PI * 1.5f, angle,       thickness);
//             draw_arc(&imgainim, a, {float(hs), float(hs)}, radius - 1, angle,       M_PI * 0.5f, thickness);
//         }
//     }
}

}//namespace r64fx