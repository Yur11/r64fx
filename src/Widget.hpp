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
class KeyPressEvent;
class KeyReleaseEvent;
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
    
    /* Effectivly calls setParent on the given widget. */
    void add(Widget* child);

    Widget* root();


/* === Geometry. === */

    void setPosition(Point<int> pos);

    Point<int> position() const;
    
    void setSize(Size<int> size);

    Size<int> size() const;

    int width() const;

    int height() const;

    Rect<int> rect() const;

    Point<int> toRootCoords(Point<int> point) const;

    Rect<int> toRootCoords(Rect<int> rect) const;

    bool isVisible() const;


/* === Window === */

    /* Show this widget in a window. */
    void show();

    /* Hide the window. */
    void hide();

    /* Close the window. */
    void close();

    Window* window() const;

    bool isWindow() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;


/* === Mouse === */

    void grabMouse();

    static void ungrabMouse();

    static Widget* mouseGrabber();

    bool isMouseGrabber() const;


/* === Keyboard === */
    void setFocusOnClick(bool yes);

    bool gainsFocusOnClick() const;

    void setFocus();

    static void removeFocus();

    static Widget* focusOwner();

    bool hasFocus() const;

    void startTextInput();

    void stopTextInput();

    bool doingTextInput();


/* === Update/Reconfigure cycle === */

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
    /* Secondary Events */
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void focusInEvent();

    virtual void focusOutEvent();

    /* Window Events */
    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

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
    friend void window_text_input      (Window* window, const std::string &text, int key);
    friend void window_close           (Window* window);
};
    
}//namespace r64fx

#endif//R64FX_GUI_WIDGET_H