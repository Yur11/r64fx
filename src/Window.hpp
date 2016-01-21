#ifndef R64FX_WINDOW_HPP
#define R64FX_WINDOW_HPP

#include <string>
#include <vector>
#include "Rect.hpp"
#include "ClipboardMode.hpp"

namespace r64fx{

class Image;
class ClipboardDataType;

class Window{
    void* m_data = nullptr;

public:
    enum class Type{
        Image
#ifdef R64FX_USE_GL
        ,GL
#endif//R64FX_USE_GL
    };

    struct Events{
        /* A window has been resized. */
        void (*resize)                (Window* window, int width, int height);

        /* Mouse pointer moved. */
        void (*mouse_press)           (Window* window, int x, int y, unsigned int button);
        void (*mouse_release)         (Window* window, int x, int y, unsigned int button);
        void (*mouse_move)            (Window* window, int x, int y);

        /* A key has been pressed. */
        void (*key_press)      (Window* window, unsigned int key);

        /* A key has been released. */
        void (*key_release)    (Window* window, unsigned int key);

        /* New text input.
         * Use startTextInput(), stopTextInput() and doingTextInput() methods. */
        void (*text_input)     (Window* window, const std::string &text, unsigned int key);

        void (*clipboard_metadata) (Window* window, ClipboardDataType* types, int ntypes, ClipboardMode mode);
        void (*clipboard_data) (Window* window, ClipboardDataType* types, int ntypes, ClipboardMode mode);

//         void (*drag_enter)    (Window* window);
//         void (*drag_move)     (Window* window, int x, int y);
//         void (*drag_leave)    (Window* window);
//         void (*drag_finished) (Window* window);
//         void (*drag_types)    (Window* window, std::string* types, int ntypes);
//         void (*drag_data)     (Window* window, void* data, int size);
//         void (*drop)          (Window* window);

        void (*close)(Window* window);
    };

    inline void setData(void* data) { m_data = data; }
    inline void* data() const { return m_data; }

private:
    Window::Type m_type;

public:
    virtual Window::Type type() = 0;

    virtual void show() = 0;

    virtual void hide() = 0;

    virtual void resize(int width, int height) = 0;

    virtual int width() = 0;

    virtual int height() = 0;


    virtual void makeCurrent() = 0;

    virtual void repaint(Rect<int>* rects = nullptr, int numrects = 0) = 0;

    virtual Image* image() = 0;


    virtual void setTitle(std::string title) = 0;

    virtual std::string title() = 0;


    virtual void startTextInput() = 0;

    virtual void stopTextInput() = 0;

    virtual bool doingTextInput() = 0;


    virtual void setSelectionData (std::string type, void* data, int data_size, bool copy_data) = 0;
    virtual void setClipboardData (std::string type, void* data, int data_size, bool copy_data) = 0;
    virtual void startDrag        (std::string type, void* data, int data_size, bool copy_data) = 0;

    virtual void requestSelectionTypes() = 0;
    virtual void requestClipboardTypes() = 0;
    virtual void requestDragTypes()      = 0;

    virtual void requestSelectionData (std::string type) = 0;
    virtual void requestClipboardData (std::string type) = 0;
    virtual void requestDragData      (std::string type) = 0;

    static Window* newInstance(
        int width = 800, int height = 600,
        std::string title = "",
        Window::Type type = Window::Type::Image
    );

    static void deleteInstance(Window* window);

    static void processSomeEvents(Window::Events* events);

    static void forEach(void (*fun)(Window* window, void* data), void* data);
};

};

#endif//R64FX_WINDOW_HPP