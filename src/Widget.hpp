#ifndef R64FX_WIDGET_HPP
#define R64FX_WIDGET_HPP

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"
#include "Mouse.hpp"
#include "Clipboard.hpp"
#include "Orientation.hpp"
#include "Window.hpp"

namespace r64fx{

class Widget;
class WidgetImpl;
class Window;
class Painter;
class MousePressEvent;
class MouseReleaseEvent;
class MouseMoveEvent;
class KeyPressEvent;
class KeyReleaseEvent;
class TextInputEvent;
class ClipboardMetadata;
class ClipboardDataRecieveEvent;
class ClipboardDataTransmitEvent;
class ClipboardMetadataRecieveEvent;
class DndEnterEvent;
class DndLeaveEvent;
class DndMoveEvent;
class DndDropEvent;

typedef LinkedList<Widget>::Iterator WidgetIterator;

class Widget : public LinkedList<Widget>::Node{
    friend class WidgetImpl;
    friend class WindowEventDispatcher;

    /* Widgets parent can be either a widget or a window.
     * Never both at same time. */
    union{
        Widget* widget = nullptr;
        Window* window;
    } m_parent;

    /* Position relative to the parent.
     * {0, 0} is the top-left corner. */
    Point<int> m_position = {0, 0};

    Size<int> m_size = {0, 0};

    /* A linked list of child widgets. */
    LinkedList<Widget> m_children;

public:
    /* Child iterators. */

    inline WidgetIterator begin() const { return m_children.begin(); }

    inline WidgetIterator end() const { return nullptr; }

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

    Widget* root() const;

    /* Find leaf child at the given position. */
    Widget* leafAt(Point<int> position, Point<int>* offset = nullptr);


/* === Geometry. === */

    void setPosition(Point<int> pos);

    Point<int> position() const;

    void setX(int x);

    int x() const;

    void setY(int y);

    int y() const;
    
    void setSize(Size<int> size);

    Size<int> size() const;

    void setWidth(int width);

    int width() const;

    void setHeight(int height);

    int height() const;

    Point<int> toRootCoords(Point<int> point, Widget** root = nullptr);

    void recomputeChildrenVisibility(const Rect<int> &clip_rect);

    void recomputeChildrenVisibility();

    bool isVisible() const;

    virtual Point<int> contentOffset();

    Rect<int> childrenBoundingRect() const;


/* === Window === */

    /* Show this widget in a window. */
    void show(
        Window::WmType  wm_type        = Window::WmType::Normal,
        Window::Type    type           = Window::Type::GL,
        Window*         modal_parent   = nullptr
    );

    /* Hide the window. */
    void hide();

    /* Close the window. */
    void close();

    Window* window() const;

    Window* rootWindow() const;

    bool isWindow() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;


/* === Extra Flags === */

    void setOrientation(Orientation orientation);

    Orientation orientation() const;

    void setPinned(bool yes);

    bool isPinned() const;


/* === Mouse === */

    void grabMouse();

    static void ungrabMouse();

    static Widget* mouseGrabber();

    bool isMouseGrabber() const;

    static MouseButton pressedButtons();

    void initMousePressEvent(
        Point<int> event_position,
        MouseButton button,
        bool ignore_grabs = false,
        bool ignore_self  = false
    );

    void initMouseReleaseEvent(
        Point<int> event_position,
        MouseButton button,
        bool ignore_grabs = false,
        bool ignore_self  = false
    );

    Widget* initMouseMoveEvent(
        Point<int> event_position,
        Point<int> event_delta,
        MouseButton pressed_buttons,
        Widget* moused_over_widget,
        bool ignore_grabs = false,
        bool ignore_self  = false
    );

    void initMouseEnterEvent();

    void initMouseLeaveEvent();

    bool isHovered();


/* === Keyboard === */

    void setFocus();

    static void removeFocus();

    static Widget* focusOwner();

    bool hasFocus() const;

    void startTextInput();

    void stopTextInput();

    bool doingTextInput();

    void initKeyPressEvent(unsigned int key);

    void initKeyReleaseEvent(unsigned int key);

    void initTextInputEvent(const std::string &text, unsigned int key);


/* === Clipboard, Selections, Drag and Drop === */

    void anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode);

    void requestClipboardMetadata(ClipboardMode mode);

    void requestClipboardData(ClipboardDataType type, ClipboardMode mode);

    void startDrag(Widget* drag_object, Point<int> anchor);

/* === Paint cycle === */

    /* Request a clipping for this widget and it's children. */
    void clip();

    /* Request a repaint for this widget. */
    void repaint();

    /* PaintEvent structure passed to the paintEvent() method. */
    class PaintEvent{
        friend class WidgetImpl;

        WidgetImpl* m_impl = nullptr;

        PaintEvent(WidgetImpl* impl)
        : m_impl(impl)
        {}

        PaintEvent(const PaintEvent&) {}

    public:
        WidgetImpl* impl() const;

        Painter* painter() const;
    };


    class ResizeEvent{
        Size<int> m_size;

        ResizeEvent(const ResizeEvent&) {}

    public:
        ResizeEvent(Size<int> size) : m_size(size) {}

        inline Size<int> size() const { return m_size; }

        inline int width()  const { return m_size.width(); }

        inline int height() const { return m_size.height(); }
    };


    class ClipEvent{
        Rect<int> m_rect;

    public:
        ClipEvent(Rect<int> rect) : m_rect(rect) {}

        inline Rect<int> rect() const { return m_rect; }
    };


protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void clipEvent(ClipEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();

    virtual void focusInEvent();

    virtual void focusOutEvent();

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

    virtual void textInputEvent(TextInputEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);

    virtual void dndEnterEvent(DndEnterEvent* event);

    virtual void dndLeaveEvent(DndLeaveEvent* event);

    virtual void dndMoveEvent(DndMoveEvent* event);

    virtual void closeEvent();

private:
    bool childrenMousePressEvent(MousePressEvent* event);

    bool childrenMouseReleaseEvent(MouseReleaseEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_GUI_WIDGET_HPP