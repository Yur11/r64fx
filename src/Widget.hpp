#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"

namespace r64fx{

class Window;
class Painter;
class MousePressEvent;
class MouseReleaseEvent;
class MouseMoveEvent;
class KeyEvent;
class TextInputEvent;

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

    /* Bit-packed bool flags.
     * These can be used by the base class as well as by derived classes. Hence protected access.
     * See WidgetFlags.hpp */
    unsigned long m_flags = 0;

public:
    Widget(Widget* parent = nullptr);
    
    virtual ~Widget();

    /* Set parent for this widget.
     * This widget is added to the list of children
     * of the given parent. */
    void setParent(Widget* parent);
    
    Widget* parent() const;

    /* Returns parent window that hosts this widget
     * or nullptr if this widget has no parent window. */
    Window* parentWindow() const;
    
    /* Effectivly calls setParent on the given widget. */
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

    /* Hide the window. */
    void hide();

    /* Close the window. */
    void close();



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

    /* Request an update for this widget.
     * This will result in reconfigure() being called for this widget
     * with the next processEvents() invocation. */
    void update();

    /* Iface. passed down the reconfigure call tree. */
    class ReconfigureEvent{
        ReconfigureEvent(const ReconfigureEvent&) {}

    protected:
        ReconfigureEvent() {}

    public:
        /* Painter to be used by reconfigure implementation. */
        Painter* painter();

        /* Widgets visible rect. as clipped by the parent structures. */
        Rect<int> visibleRect();
    };

    static void processEvents();

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyEvent* event);

    virtual void keyReleaseEvent(KeyEvent* event);

    virtual void textInputEvent(TextInputEvent* event);

    virtual void closeEvent();

private:
    void reconfigureChildren(ReconfigureEvent* event);

    friend void process_window_updates (Window* window, void*);
    friend void window_resize          (Window* window, int width, int height);
    friend void window_mouse_press     (Window* window, int x, int y, unsigned int button);
    friend void window_mouse_release   (Window* window, int x, int y, unsigned int button);
    friend void window_mouse_move      (Window* window, int x, int y);
    friend void window_key_press       (Window* window, int key);
    friend void window_key_release     (Window* window, int key);
    friend void window_text_input      (Window* window, char* utf8, unsigned int size, int key);
    friend void window_close           (Window* window);
};
    
}//namespace r64fx

#endif//R64FX_GUI_WIDGET_H