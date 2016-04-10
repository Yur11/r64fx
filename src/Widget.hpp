#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include <string>
#include "LinkedList.hpp"
#include "Rect.hpp"
#include "Mouse.hpp"
#include "Clipboard.hpp"
#include "Orientation.hpp"

namespace r64fx{

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
    inline LinkedList<Widget>::Iterator begin() const { return m_children.begin(); }

    inline LinkedList<Widget>::Iterator end() const { return nullptr; }

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

    Widget* leafAt(Point<int> position, Point<int>* offset = nullptr);


/* === Geometry. === */

    void setPosition(Point<int> pos);

    Point<int> position() const;
    
    void setSize(Size<int> size);

    Size<int> size() const;

    void setWidth(int width);

    int width() const;

    void setHeight(int height);

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

    Window* rootWindow() const;

    bool isWindow() const;

    void setWindowTitle(std::string title);

    std::string windowTitle() const;


// /* === Extra Flags === */
//
//     void setOrientation(Orientation orientation);
//
//     Orientation orientation() const;
//
//     void setPinned(bool yes);
//
//     bool isPinned() const;


/* === Mouse === */

    void grabMouse();

    static void ungrabMouse();

    static Widget* mouseGrabber();

    bool isMouseGrabber() const;

    static MouseButton pressedButtons();

    bool grabsMouseOnClick(bool yes);

    bool grabsMouseOnClick() const;

    void initMousePressEvent(Point<int> event_position, MouseButton button);

    void initMouseReleaseEvent(Point<int> event_position, MouseButton button);

    void initMouseMoveEvent(Point<int> event_position);


/* === Keyboard === */

    bool getsFocusOnClick(bool yes);

    bool getsFocusOnClick() const;

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


/* === Update/Reconfigure cycle === */

    /* Request an update for this widget. */
    void update();


    class UpdateEvent{
        UpdateEvent(const UpdateEvent&) {}

    protected:
        UpdateEvent() {}

    public:
        Painter* painter();

        void setOffset(Point<int> offset);

        Point<int> offset() const;

        /* Widgets visible rect. as clipped by the parent structures. */
        Rect<int> visibleRect();
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

    static void processEvents();

protected:
    virtual void updateEvent(UpdateEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

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
    void updateChildren(UpdateEvent* event);

    friend void process_window_updates(Window* window, void*);
    friend class WindowEvents_Widget;
};
    
}//namespace r64fx

#endif//R64FX_GUI_WIDGET_H