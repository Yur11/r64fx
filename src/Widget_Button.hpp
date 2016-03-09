#ifndef R64FX_WIDGET_BUTTON_HPP
#define R64FX_WIDGET_BUTTON_HPP

#include "Widget.hpp"

namespace r64fx{

class ImageAnimation;

class Widget_Button : public Widget{
    ImageAnimation* m_animation = nullptr;
    int m_frame = 0;
    void (*m_on_click)(Widget_Button* button, void* data);
    void* m_on_click_data = nullptr;

public:
    Widget_Button(Widget* parent = nullptr);

    Widget_Button(std::string text, std::string font_name = "", Widget* parent = nullptr);

    virtual ~Widget_Button();

    void onClick(void (*callback)(Widget_Button* button, void* data), void* data = nullptr);

    void setState(int frame);

public:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_BUTTON_HPP