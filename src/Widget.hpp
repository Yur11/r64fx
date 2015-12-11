#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"

namespace r64fx{

class ReconfContext;
class MousePressEvent;
class MouseReleaseEvent;
class MouseMoveEvent;
class KeyEvent;
class Window;
class Image;

class Widget : public LinkedList<Widget>::Node{
    friend class Program;

    union{
        Widget* widget = nullptr;
        Window* window;
    } m_parent;
    
    /* Widgets geometry. */
    Rect<int> m_rect = { 0, 0, 0, 0 };

    /* A linked list of widgets children. */
    LinkedList<Widget> m_children;

protected:
    /* Storage for widget flags.
     * These are bit-packed and used internally.
     */
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
    
    /** @brief Show this widget in a window. */
    void show();

    /** @brief Hide this widget if it is shown in a window.*/
    void hide();

    void close();

    bool isWindow() const;

    bool isVisible() const;

    bool isObscuredLeft() const;

    bool isObscuredTop() const;

    bool isObscuredRight() const;

    bool isObscuredBottom() const;

    bool isPartiallyObscured() const;

    void grabMouse();

    static void ungrabMouse();

    static Widget* mouseGrabber();

    bool isMouseGrabber() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;

    Window* rootWindow() const;

    Point<int> toRootCoords(Point<int> point) const;

    Rect<int> toRootCoords(Rect<int> rect) const;

protected:
    virtual void reconfigure(ReconfContext* ctx);

    virtual void mousePressEvent(MousePressEvent* event);
    
    virtual void mouseReleaseEvent(MouseReleaseEvent* event);
    
    virtual void mouseMoveEvent(MouseMoveEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    virtual void keyReleaseEvent(KeyEvent* event);

    void update();

private:
    void reconfigureChildren(ReconfContext* ctx);

};
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H