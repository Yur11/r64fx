#ifndef R64FX_WIDGET_SLIDER_HPP
#define R64FX_WIDGET_SLIDER_HPP

#include "Widget.hpp"
#include "Value.hpp"

namespace r64fx{

class SliderHandle;

class Widget_Slider : public Widget, public Value{
    SliderHandle* m_handle = nullptr;

public:
    Widget_Slider(int length, int width, Orientation orientation, Widget* parent = nullptr);

    virtual ~Widget_Slider();

protected:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

private:
    int travelDistance() const;

    void setValueFromPosition(Point<int> position);

    int positionFromValue(float value) const;
};

}//namespace r64fx

#endif//R64FX_WIDGET_SLIDER_HPP
