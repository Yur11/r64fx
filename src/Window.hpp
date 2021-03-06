#ifndef R64FX_WINDOW_HPP
#define R64FX_WINDOW_HPP

#include <string>
#include "GeometryUtils.hpp"
#include "Clipboard.hpp"

namespace r64fx{

class Window;
class WindowEventDispatcherIface;
class Image;
class ClipboardDataType;


class Window{
    void* m_data = nullptr;

public:
    enum class WmType{
        Normal,
        Menu,
        Dialog,
        ToolTip,
        DND,
    };

    enum class Type{
        Default,
        Best,
        Image
#ifdef R64FX_USE_GL
        ,GL
#endif//R64FX_USE_GL
    };

    enum class CursorType{
        Arrow,
        Busy,
        BusyArrow,
        IBeam,
        Cross,
        PointingHand,
        OpenHand,
        ClosedHand,
        DndDrop,
        DndNoDrop,
        ResizeNS,
        ResizeWE,
        ResizeDiag1,
        ResizeDiag2,
        Fleur,
        LeftSide,
        RightSide,
        TopSide,
        BottomSide,
        CornerTopLeft,
        CornerTopRight,
        CornerBottomLeft,
        CornerBottomRight,
        Pencil,
        Forbidden,
        Skull,
        X
    };

    inline void setData(void* data) { m_data = data; }
    inline void* data() const { return m_data; }

private:
    Window::Type m_type;

public:
    virtual Window::Type type() = 0;

    virtual void show() = 0;

    virtual void hide() = 0;

    virtual void setPosition(Point<int> position) = 0;

    virtual Point<int> position() = 0;

    inline int x() { return position().x(); }

    inline int y() { return position().y(); }

    virtual void setSize(Size<int> size) = 0;

    virtual Size<int> size() = 0;

    inline int width()  { return size().width(); }

    inline int height() { return size().height(); }

    virtual Size<int> getScreenSize() = 0;


    virtual void makeCurrent() = 0;

    virtual void repaint(Rect<int>* rects = nullptr, int numrects = 0) = 0;

    virtual Image* image() = 0;


    virtual void setTitle(std::string title) = 0;

    virtual std::string title() = 0;

    virtual void setWmType(Window::WmType wm_type) = 0;

    virtual void showDecorations(bool yes) = 0;

    virtual void setModalTo(Window* window) = 0;


    virtual void setCursorType(CursorType shape) = 0;


    virtual void grabMouseFocus() = 0;

    virtual void releaseMouseFocus() = 0;


    virtual void startTextInput() = 0;

    virtual void stopTextInput() = 0;

    virtual bool doingTextInput() = 0;


    virtual void anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode) = 0;

    virtual void requestClipboardData(ClipboardDataType type, ClipboardMode mode) = 0;

    virtual void requestClipboardMetadata(ClipboardMode mode) = 0;


    virtual void startDrag(const ClipboardMetadata &metadata, Window* drag_object, int anchor_x, int anchor_y) = 0;


    static Window* newInstance(
        int width = 800, int height = 600,
        Window::Type    type     = Window::Type::Image,
        Window::WmType  wm_type  = Window::WmType::Normal
    );

    static void deleteInstance(Window* window);

    static void processSomeEvents(WindowEventDispatcherIface* events);

    static void forEach(void (*fun)(Window* window, void* data), void* data);
};


class WindowEventDispatcherIface{
public:
    virtual void resizeEvent(Window* window, int width, int height) = 0;

    virtual void mousePressEvent   (Window* window, int x, int y, unsigned int button) = 0;
    virtual void mouseReleaseEvent (Window* window, int x, int y, unsigned int button) = 0;
    virtual void mouseMoveEvent    (Window* window, int x, int y) = 0;

    virtual void mouseEnterEvent   (Window* window) = 0;
    virtual void mouseLeaveEvent   (Window* window) = 0;

    virtual void keyPressEvent     (Window* window, unsigned int key) = 0;
    virtual void keyReleaseEvent   (Window* window, unsigned int key) = 0;

    virtual void textInputEvent    (Window* window, const std::string &text, unsigned int key) = 0;

    virtual void clipboardDataRecieveEvent(
        Window* window,
        ClipboardDataType type, ClipboardMode mode,
        void* data, int size
    ) = 0;

    virtual void clipboardDataTransmitEvent(
        Window* window,
        ClipboardDataType type, ClipboardMode mode,
        void (*on_transmit)(Window* window, void* data, int size)
    ) = 0;

    virtual void clipboardMetadataRecieveEvent(
        Window* window,
        const ClipboardMetadata &metadata, ClipboardMode mode
    ) = 0;

    virtual void dndMoveEvent     (Window* window, int x, int y, const ClipboardMetadata& metadata, bool &out_accepted) = 0;
    virtual void dndDropEvent     (Window* window, const ClipboardMetadata& metadata, ClipboardDataType &out_data_type, bool &out_accepted) = 0;
    virtual void dndLeaveEvent    (Window* window) = 0;
    virtual void dndReleaseEvent  () = 0;
    virtual void dndFinishedEvent () = 0;

    virtual void closeEvent(Window* window) = 0;
};

};

#endif//R64FX_WINDOW_HPP
