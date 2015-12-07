#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"

namespace r64fx{

class MouseEvent;
class KeyEvent;
class ReconfigureEvent;
class ShowEvent;
class Window;
class Image;
class Painter;

class Widget : public LinkedList<Widget>::Node{
    friend class Program;

    union{
        Widget* widget = nullptr;
        Window* window;
    } m_parent;
    
    /* Widgets geometry. */
    Rect<int> m_rect = { 0, 0, 0, 0  };

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

    Image* windowImage() const;

    bool isVisible() const;

    bool isObscuredLeft() const;

    bool isObscuredTop() const;

    bool isObscuredRight() const;

    bool isObscuredBottom() const;

    bool isPartiallyObscured() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;

protected:
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    virtual void keyReleaseEvent(KeyEvent* event);

    virtual void reconfigureEvent(ReconfigureEvent* event);
};
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H