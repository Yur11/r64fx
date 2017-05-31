#ifndef R64FX_WIDGET_SLIDER_HPP
#define R64FX_WIDGET_SLIDER_HPP

#include "Widget.hpp"
#include "Value.hpp"

namespace r64fx{

class Widget_Slider : public Widget, public Value{
public:
    Widget_Slider(int length, Orientation orientation, Widget* parent = nullptr);

    virtual ~Widget_Slider();

    void setValueFromPosition(Point<int> position);

    int barLength() const;

    int barOffset() const;

    bool barVisible(bool yes);

    bool barVisible() const;

    bool isFlipped(bool yes);

    bool isFlipped() const;

    bool isReversed(bool yes);

    bool isReversed() const;

protected:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_SLIDER_HPP
