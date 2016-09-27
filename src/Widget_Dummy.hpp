#ifndef R64FX_WIDGET_DUMMY_HPP
#define R64FX_WIDGET_DUMMY_HPP

#include "Widget.hpp"
#include "Image.hpp"

namespace r64fx{

class Widget_Dummy : public Widget{
    unsigned char m_color[4] = {0, 0, 0, 0};
    Image m_img;

public:
    Widget_Dummy(Widget* parent = nullptr);

    virtual ~Widget_Dummy();

private:
    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

    virtual void closeEvent();

    void changeColor();
};

}//namespace r64fx

#endif//R64FX_WIDGET_DUMMY_HPP
