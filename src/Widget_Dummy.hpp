#ifndef R64FX_WIDGET_DUMMY_HPP
#define R64FX_WIDGET_DUMMY_HPP

#include "Widget.hpp"
#include "Color.hpp"

namespace r64fx{

class Widget_Dummy : public Widget{
    Color<unsigned char> m_Color;

public:
    Widget_Dummy(Color<unsigned char> color, Widget* parent = nullptr);

    virtual ~Widget_Dummy();

    virtual void reconfigure(ReconfContext* ctx);

protected:
    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyEvent* event);

    virtual void keyReleaseEvent(KeyEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_DUMMY_HPP
