#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"

namespace r64fx{

class Window;
class Image;
class Painter;
class MousePressEvent;
class MouseReleaseEvent;
class MouseMoveEvent;
class KeyEvent;


class Widget : public LinkedList<Widget>::Node{

    /* Widgets parent can be either a widget or a window.
     * Never both at same time. */
    union{
        Widget* widget = nullptr;
        Window* window;
    } m_parent;
    
    /* Widgets geometry.
     * Position in parents coordinate space + widget size. */
    Rect<int> m_rect = { 0, 0, 0, 0 };

    /* A linked list of widgets children. */
    LinkedList<Widget> m_children;

protected:

    /* Storage for widget flags.
     * These are bit-packed and used internally. */
    unsigned long m_flags = 0;

public:
    Widget(Widget* parent = nullptr);
    
    virtual ~Widget();

    void setParent(Widget* parent);
    
    Widget* parent() const;

    Window* parentWindow() const;
    
    void add(Widget* child);

    Widget* root();

    void setPosition(Point<int> pos);

    Point<int> position() const;
    
    void setSize(Size<int> size);

    Size<int> size() const;

    int width() const;

    int height() const;

    Rect<int> rect() const;
    
    /* Show this widget in a window. */
    void show();

    /* Hide this widget if it is shown in a window.*/
    void hide();

    void close();

    void update();

    bool isWindow() const;

    bool isVisible() const;

    void grabMouse();

    static void ungrabMouse();

    static Widget* mouseGrabber();

    bool isMouseGrabber() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;

    Point<int> toRootCoords(Point<int> point) const;

    Rect<int> toRootCoords(Rect<int> rect) const;

    /* Iface. passed down the reconfigure call tree. */
    class ReconfContext{
    public:
        /* Painter to be used by reconfigure implementation. */
        Painter* painter();

        /* Widgets visible rect. as clipped by the parent structures. */
        Rect<int> visibleRect();
    };

    static void processWindowResize(Window* window);

    static void initReconf(Window* window);

protected:
    virtual void reconfigure(Widget::ReconfContext* ctx);

private:
    void reconfigureChildren(ReconfContext* ctx);


public:
    static void initMousePressEvent(Window* window, MousePressEvent* event);

protected:
    virtual void mousePressEvent(MousePressEvent* event);


public:
    static void initMouseReleaseEvent(Window* window, MouseReleaseEvent* event);

protected:
    virtual void mouseReleaseEvent(MouseReleaseEvent* event);


public:
    static void initMouseMoveEvent(Window* window, MouseMoveEvent* event);

protected:
    virtual void mouseMoveEvent(MouseMoveEvent* event);


public:
    static void initKeyPressEvent(Window* window, KeyEvent* event);

protected:
    virtual void keyPressEvent(KeyEvent* event);


public:
    static void initKeyReleaseEvent(Window* window, KeyEvent* event);

protected:
    virtual void keyReleaseEvent(KeyEvent* event);

};
    
}//namespace r64fx

#endif//R64FX_GUI_WIDGET_H