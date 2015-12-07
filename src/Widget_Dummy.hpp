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

protected:
    virtual void mousePressEvent(MouseEvent* event);

    virtual void mouseReleaseEvent(MouseEvent* event);

    virtual void mouseMoveEvent(MouseEvent* event);

    virtual void keyPressEvent(KeyEvent* event);

    virtual void keyReleaseEvent(KeyEvent* event);

    virtual void reconfigureEvent(ReconfigureEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_DUMMY_HPP
