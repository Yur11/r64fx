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
class DndMoveEvent;
class DndDropEvent;
class DndLeaveEvent;
class DndReleaseEvent;
class DndFinishedEvent;

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

protected:
    /* Bit-packed bool flags.
     * These can be used by the base class as well as by derived classes. Hence protected access.
     * See WidgetFlags.hpp */
    unsigned long m_flags = 0;

public:
    Widget(Widget* parent = nullptr);

    virtual ~Widget();

    void setParent(Widget* parent, bool insert_after = true, Widget* existing_child = nullptr);

    /* Parent widget or nullptr if this widget has no parent or is a window. */
    Widget* parent() const;

    /* Adds a widget to the beginning of the children list.
       Same as calling child->setParent(this, false);
     */
    void preppend(Widget* child);

    /* Adds a widget to the end of the children list.
       Same as calling child->setParent(this, true);
     */
    void append(Widget* child);

    /* Adds a widget before an existing child.
       Same as calling child->setParent(this, false, existing_child);
     */
    void insertBefore(Widget* existing_child, Widget* child);

    /* Adds a widget after an existing child.
       Same as calling child->setParent(this, true, existing_child);
     */
    void insertAfter(Widget* existing_child, Widget* child);

    /* Iterator pointing to the first child. */
    WidgetIterator begin() const;

    /* Iterator pointing past the last child. */
    WidgetIterator end() const;

    bool hasChildren() const;

    /* Remove the first child and return it. */
    Widget* popFirstChild();

    /* The root parent widget in a tree that this widget belongs to. */
    Widget* root() const;

    void setPosition(Point<int> pos);

    /* Position of this widget in the coordinate system of the parent widget. */
    Point<int> position() const;

    void setX(int x);

    int x() const;

    void setY(int y);

    int y() const;

    void setSize(Size<int> size, bool send_event = true);

    Size<int> size() const;

    void setWidth(int width, bool send_event = true);

    int width() const;

    void setHeight(int height, bool send_event = true);

    int height() const;

    /* Find leaf child at a given position.
     * The optional offset parameter is used to return the leaf position
     * in the coordinate system of this widget. */
    Widget* leafAt(Point<int> position, Point<int>* offset = nullptr);

    /* Convert a point from the coordinate system of this widget
     * to the coordinate system of the root widget.
     * The optional root parameter is used to obtain the root widget. */
    Point<int> toRootCoords(Point<int> point, Widget** root = nullptr);

    /* Iterate over the children and find whether they are visible inside the given clip_rect.
     * The clip_rect is used in the coordinate system of this widget with contentOffset() applied. */
    void recomputeChildrenVisibility(const Rect<int> &clip_rect);

    /* Calls recomputeChildrenVisibility() with clip_rect = {0, 0, width(), height()}. */
    void recomputeChildrenVisibility();

    /* Visiblity flag updated by calling recomputeChildrenVisibility() on the parent widget. */
    bool isVisible() const;

    /* Offset added to positions of child widgets
     * when processing them in geometry algorithms.
     * Used to implement scrolling. */
    virtual Point<int> contentOffset();

    /* A rectangle that encloses all the children of this widget.
     * Without contentOffset() applied. */
    Rect<int> childrenBoundingRect() const;

    void setOrientation(Orientation orientation);

    Orientation orientation() const;

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

    /* The window that this widget is shown in or nullptr. */
    Window* window() const;

    /* The window that the root widget is show in or nullptr. */
    Window* rootWindow() const;

    bool isWindow() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;

    void grabMouse();

    static void ungrabMouse();

    static Widget* mouseGrabber();

    bool isMouseGrabber() const;

    void grabMouseForMultipleWidgets();

    void ungrabMouseForMultipleWidgets();

    bool wantsMultiGrabs(bool yes);

    bool wantsMultiGrabs();
    
    static Widget* mouseMultiGrabber();

    static MouseButton pressedButtons();

    bool isHovered();

    void setFocus();

    static void removeFocus();

    static Widget* focusOwner();

    bool hasFocus() const;

    void startTextInput();

    void stopTextInput();

    bool doingTextInput();

    void anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode);

    void requestClipboardMetadata(ClipboardMode mode);

    void requestClipboardData(ClipboardDataType type, ClipboardMode mode);

    void startDrag(const ClipboardMetadata &metadata, Widget* dnd_object, Point<int> anchor);

    bool dndEnabled(bool yes);

    bool dndEnabled() const;

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
        Size<int> m_old_size;
        Size<int> m_new_size;

        ResizeEvent(const ResizeEvent&) {}

    public:
        ResizeEvent(Size<int> old_size, Size<int> new_size)
        : m_old_size(old_size)
        , m_new_size(new_size)
        {}

        inline Size<int> size() const { return m_new_size; }

        inline Size<int> oldSize() const { return m_old_size; }

        inline int width()  const { return m_new_size.width(); }

        inline int height() const { return m_new_size.height(); }

        inline int oldWidth() const { return m_old_size.width(); }

        inline int oldHeight() const { return m_old_size.height(); }

        inline bool widthChanged() const { return width() != oldWidth(); }

        inline bool heightChanged() const { return height() != oldHeight(); }
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

private:
    bool childrenMousePressEvent(MousePressEvent* event);

protected:
    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

private:
    bool childrenMouseReleaseEvent(MouseReleaseEvent* event);

protected:
    virtual void mouseMoveEvent(MouseMoveEvent* event);
    
    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();

    virtual void clickedElsewhereEvent();

    virtual void focusInEvent();

    virtual void focusOutEvent();

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

    virtual void textInputEvent(TextInputEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);

    virtual void dndMoveEvent(DndMoveEvent* event);

    virtual void dndDropEvent(DndDropEvent* event);

    virtual void dndLeaveEvent(DndLeaveEvent* event);

    virtual void dndReleaseEvent(DndReleaseEvent* event);

    virtual void dndFinishedEvent(DndFinishedEvent* event);

    virtual void closeEvent();
};

}//namespace r64fx

#endif//R64FX_GUI_WIDGET_HPP
