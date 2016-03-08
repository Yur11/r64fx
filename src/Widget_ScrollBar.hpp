#ifndef R64FX_WIDGET_SCROLL_BAR_HPP
#define R64FX_WIDGET_SCROLL_BAR_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ScrollBar : public Widget{
protected:
    float m_ratio            = 1.0f;
    float m_handle_position  = 0.0f;
    void(*m_position_changed)(Widget_ScrollBar* scroll_bar, void* data);
    void* m_position_changed_data = nullptr;

public:
    Widget_ScrollBar(Widget* parent = nullptr);

    void setRatio(float ratio);

    float ratio() const;

    void setHandlePosition(float position);

    float handlePosition() const;

    void onPositionChanged(void(*callback)(Widget_ScrollBar* scroll_bar, void* data), void* data);
};


class Widget_ScrollBar_Vertical : public Widget_ScrollBar{
public:
    Widget_ScrollBar_Vertical(Widget* parent = nullptr);

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};


class Widget_ScrollBar_Horizontal : public Widget_ScrollBar{
public:
    Widget_ScrollBar_Horizontal(Widget* parent = nullptr);

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_SCROLL_BAR_HPP