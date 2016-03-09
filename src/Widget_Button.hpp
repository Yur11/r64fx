#ifndef R64FX_WIDGET_BUTTON_HPP
#define R64FX_WIDGET_BUTTON_HPP

#include "Widget.hpp"

namespace r64fx{

class ImageAnimation;

class Widget_Button : public Widget{
    ImageAnimation*  m_animation      = nullptr;
    int              m_state          = 0;
    void           (*m_on_click)(Widget_Button* button, void* data);
    void*            m_on_click_data  = nullptr;

public:
    Widget_Button(Widget* parent = nullptr);

    Widget_Button(std::string text, std::string font_name = "", Widget* parent = nullptr);

    Widget_Button(ImageAnimation* animation, Widget* parent = nullptr);

    virtual ~Widget_Button();

    void onClick(void (*callback)(Widget_Button* button, void* data), void* data = nullptr);

    void setState(int i);

    int state();

    void pickNextStateLoop();

public:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);
};


ImageAnimation* new_button_animation(int width, int height, unsigned char** colors, int nframes);

}//namespace

#endif//R64FX_WIDGET_BUTTON_HPP