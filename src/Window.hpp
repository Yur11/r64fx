#ifndef R64FX_WINDOW_HPP
#define R64FX_WINDOW_HPP

#include <string>
#include <vector>
#include "Rect.hpp"
#include "Clipboard.hpp"

namespace r64fx{

class Window;
class WindowEvents;
class Image;
class ClipboardDataType;


class Window{
    void* m_data = nullptr;

public:
    enum class WmType{
        Normal,
        Menu,
        Dialog,
        ToolTip
    };

    enum class Type{
        Image
#ifdef R64FX_USE_GL
        ,GL
#endif//R64FX_USE_GL
    };

    inline void setData(void* data) { m_data = data; }
    inline void* data() const { return m_data; }

private:
    Window::Type m_type;

public:
    virtual Window::Type type() = 0;

    virtual void show() = 0;

    virtual void hide() = 0;

    virtual void setPosition(int x, int y) = 0;

    virtual int x() = 0;

    virtual int y() = 0;

    virtual void resize(int width, int height) = 0;

    virtual int width() = 0;

    virtual int height() = 0;


    virtual void makeCurrent() = 0;

    virtual void repaint(Rect<int>* rects = nullptr, int numrects = 0) = 0;

    virtual Image* image() = 0;


    virtual void setTitle(std::string title) = 0;

    virtual std::string title() = 0;

    virtual void setWmType(Window::WmType wm_type) = 0;

    virtual void showDecorations(bool yes) = 0;

    virtual void setModalTo(Window* window) = 0;


    virtual void startTextInput() = 0;

    virtual void stopTextInput() = 0;

    virtual bool doingTextInput() = 0;


    virtual void anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode) = 0;

    virtual void requestClipboardData(ClipboardDataType type, ClipboardMode mode) = 0;

    virtual void requestClipboardMetadata(ClipboardMode mode) = 0;

    static Window* newInstance(
        int width = 800, int height = 600,
        Window::Type    type     = Window::Type::Image,
        Window::WmType  wm_type  = Window::WmType::Normal
    );

    static void deleteInstance(Window* window);

    static void processSomeEvents(WindowEvents* events);

    static void forEach(void (*fun)(Window* window, void* data), void* data);
};


class WindowEvents{
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

    virtual void clipboardDataRecieveEvent
                         (Window* window, ClipboardDataType type, void* data, int size, ClipboardMode mode) = 0;

    virtual void clipboardDataTransmitEvent
                         (Window* window, ClipboardDataType type, void** data, int* size, ClipboardMode mode) = 0;

    virtual void clipboardMetadataRecieveEvent
                         (Window* window, const ClipboardMetadata &metadata, ClipboardMode mode) = 0;

    virtual void dndEnterEvent    (Window* window, int x, int y) = 0;
    virtual void dndLeaveEvent    (Window* window) = 0;
    virtual void dndMoveEvent     (Window* window, int x, int y) = 0;
    virtual void dndDropEvent     (Window* window) = 0;

    virtual void closeEvent(Window* window) = 0;
};

};

#endif//R64FX_WINDOW_HPP