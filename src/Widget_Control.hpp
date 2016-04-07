#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"
#include "ImageAnimation.hpp"

namespace r64fx{

class ControlAnimation;

class Widget_Control : public Widget{
    ControlAnimation*  m_animation = nullptr;
    int                m_position  = 0;
    void             (*m_on_value_changed)(Widget_Control*, void*);
    void*              m_on_value_changed_data = nullptr;

public:
    Widget_Control(ControlAnimation* animation, Widget* parent = nullptr);

    ~Widget_Control();

    void setValue(float value);

    float value() const;

    void onValueChanged(void (*callback)(Widget_Control*, void*), void* data = nullptr);

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};


class ControlAnimationState{
    int mx = 0;
    int my = 0;

public:
    ControlAnimationState(int x, int y);

    ControlAnimationState();

    void setFrameX(int frame);
    void setFrameY(int frame);

    int frameX() const;
    int frameY() const;

    static ControlAnimationState Unchanged();
};


class ControlAnimation{
    Size<int> m_size = {0, 0};
    int m_min_position = 0;
    int m_max_position = 255;

public:
    virtual ~ControlAnimation() {}

    void setSize(Size<int> size);

    Size<int> size() const;

    inline void setMinPosition(int pos) { m_min_position = pos; }
    inline void setMaxPosition(int pos) { m_max_position = pos; }

    inline int minPosition() const { return m_min_position; }
    inline int maxPosition() const { return m_max_position; }

    int positionRange() const;

    int boundPosition(int pos) const;

    virtual void paint(ControlAnimationState state);

    virtual ControlAnimationState mousePress(Point<int> position);

    virtual ControlAnimationState mouseMove(Point<int> position, Point<int> old_position);

    virtual ControlAnimationState mouseEnter();

    virtual ControlAnimationState mouseLeave();
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