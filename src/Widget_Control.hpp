#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"
#include "ImageAnimation.hpp"

namespace r64fx{

class Image;
class Font;

struct ControlAnimationState{
    unsigned long bits = 0;

    ControlAnimationState();

    explicit ControlAnimationState(unsigned long bits);
};


class ControlAnimation{
    Size<int> m_size = {0, 0};

public:
    virtual ~ControlAnimation() {}

    void setSize(Size<int> size);

    Size<int> size() const;

    int width() const;

    int height() const;

    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mousePress(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseRelease(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta);

    virtual ControlAnimationState mouseEnter(ControlAnimationState state);

    virtual ControlAnimationState mouseLeave(ControlAnimationState state);
};


class ControlAnimation_Knob : public ControlAnimation{
protected:
    unsigned char* m_data = nullptr;

public:
    ControlAnimation_Knob(int knob_radius);

    virtual ~ControlAnimation_Knob();

private:
    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta);
};


class ControlAnimation_PlayPauseButton : public ControlAnimation{
    unsigned char* m_frames = nullptr;

public:
    ControlAnimation_PlayPauseButton();

    virtual ~ControlAnimation_PlayPauseButton();

private:
    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mousePress(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseRelease(ControlAnimationState state, Point<int> position);
};


class Widget_Control : public Widget{
    ControlAnimation*      m_animation = nullptr;
    ControlAnimationState  m_state;
    void                 (*m_on_value_changed)(Widget_Control*, void*);
    void*                  m_on_value_changed_data = nullptr;

public:
    Widget_Control(ControlAnimation* animation, Widget* parent = nullptr);

    ~Widget_Control();

    void setValue(float value);

    float value() const;

    void onValueChanged(void (*callback)(Widget_Control*, void*), void* data = nullptr);

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP