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

    auto new_state = m_animation->mousePress(m_state, event->position());
    if(new_state != m_state)
    {
        m_state = new_state;
        repaint();
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
        repaint();
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
        repaint();
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


int ControlAnimation_Knob::state2frame(ControlAnimationState state)
{
    int frame = (state.bits >> 4);
    if(frame < 0)
        frame = 0;
    else if(frame > 63)
        frame = 63;
    return frame;
}


ControlAnimationState ControlAnimation_Knob::frame2state(int frame)
{
    ControlAnimationState state;
    state.bits = frame;
    state.bits <<= 4;
    return state;
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
    unsigned char bg[4] = {255, 0, 0, 0};
    painter->fillRect({{0, 0}, size()}, bg);

    if(m_data)
    {
        int frame = state2frame(state);
        Image img(width(), height(), 1, m_data + (frame * width() * height()));

        unsigned char fg[4] = {0, 0, 0, 0};
        unsigned char* colors[1] = {fg};
        painter->blendColors({0, 0}, colors, &img);
    }
}


ControlAnimationState ControlAnimation_Knob::mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta)
{
    int frame = state2frame(state);
}


ControlAnimation_Knob_UnipolarLarge::ControlAnimation_Knob_UnipolarLarge(int size)
{
    setSize({size, size});

    int data_size = size * size * 64;
    m_data = new(std::nothrow) unsigned char[data_size];
    if(!m_data)
        return;

    for(int i=0; i<data_size; i++)
    {
        m_data[i] = (unsigned char) (rand() % 255);
    }
}

}//namespace r64fx