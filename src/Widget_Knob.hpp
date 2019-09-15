#ifndef R64FX_WIDGET_KNOB_HPP
#define R64FX_WIDGET_KNOB_HPP

#include "Widget.hpp"
#include "Value.hpp"

namespace r64fx{

class KnobAnimation;

class Widget_Knob : public Widget, public Value{
protected:
    KnobAnimation*  m_animation = nullptr;

public:
    Widget_Knob(int size, Widget* parent = nullptr);

    virtual ~Widget_Knob();

#ifdef R64FX_DEBUG
    static void debugPaint(Painter* painter, Point<int> position, int size);
#endif//R64FX_DEBUG

private:
    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event);

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event);

    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_KNOB_HPP
