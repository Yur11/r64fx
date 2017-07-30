#ifndef R64FX_WIDGET_NUMBER_HPP
#define R64FX_WIDGET_NUMBER_HPP

#include "Widget.hpp"
#include "Value.hpp"

namespace r64fx{

class Widget_Number : public Widget, public Value{
public:
    Widget_Number(Widget* parent = nullptr);

    virtual ~Widget_Number();

private:
    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event);

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event);

    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_NUMBER_HPP
