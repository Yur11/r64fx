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

    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mousePress(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseRelease(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta);

    virtual ControlAnimationState mouseEnter(ControlAnimationState state);

    virtual ControlAnimationState mouseLeave(ControlAnimationState state);
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
    virtual void updateEvent(UpdateEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();
};


class ControlAnimation_MenuItem : public ControlAnimation{
    Image* m_text_image = nullptr;

public:
    ControlAnimation_MenuItem(const std::string &caption, Font* font);

    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mouseEnter(ControlAnimationState state);

    virtual ControlAnimationState mouseLeave(ControlAnimationState state);
};



struct ControlAnimation_Knob : public ControlAnimation{
    ImageAnimation imgainim;

    virtual void repaint(int position, Painter* painter);
};


struct ControlAnimation_Knob_UnipolarLarge : public ControlAnimation_Knob{
    ControlAnimation_Knob_UnipolarLarge(int size);
};


struct ControlAnimation_Knob_BipolarLarge : public ControlAnimation_Knob{
    ControlAnimation_Knob_BipolarLarge(int size);
};


struct ControlAnimation_Knob_UnipolarSector : public ControlAnimation_Knob{
    ControlAnimation_Knob_UnipolarSector(int size);
};


struct ControlAnimation_Knob_BipolarSector : public ControlAnimation_Knob{
    ControlAnimation_Knob_BipolarSector(int size);
};


}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP