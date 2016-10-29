#ifndef R64FX_WIDGET_HPP
#define R64FX_WIDGET_HPP

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"
#include "Mouse.hpp"
#include "Orientation.hpp"
#include "Window.hpp"
#include "WidgetEvents.hpp"
#include "ClipboardEvents.hpp"

namespace r64fx{

class Widget;
class Window;
class PainterTextureManager;

typedef LinkedList<Widget>::Iterator WidgetIterator;

class Widget : public LinkedList<Widget>::Node{
    friend class WidgetImpl;
    friend class WindowEventDispatcher;

    /* Parent can either be a widget or a window.
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

    Widget* firstChild() const;

    Widget* popFirstChild();

    Widget* lastChild() const;

    Widget* popLastChild();

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
        Window::Type    type           = Window::Type::Image,
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

    void grabMouseFocus();

    static void releaseMouseFocus();

    static Widget* mouseFocusOwner();

    bool isMouseFocusOwner() const;

    void grabMouseForMultipleWidgets();

    void ungrabMouseForMultipleWidgets();

    bool wantsMultiGrabs(bool yes);

    bool wantsMultiGrabs();
    
    static Widget* mouseMultiGrabber();

    static MouseButton pressedButtons();

    bool isHovered();

    void grabKeyboardFocus();

    static void releaseKeyboardFocus();

    static Widget* keyboardFocusOwner();

    bool hasKeyboardFocus() const;

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
    
    PainterTextureManager* textureManager();

protected:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void resizeEvent(WidgetResizeEvent* event);

    virtual void clipEvent(WidgetClipEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    bool childrenMousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    bool childrenMouseReleaseEvent(MouseReleaseEvent* event);

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
