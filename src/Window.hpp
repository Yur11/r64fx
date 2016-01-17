#ifndef R64FX_WINDOW_HPP
#define R64FX_WINDOW_HPP

#include <string>
#include "Rect.hpp"

namespace r64fx{

class Widget;
class Painter;
class Image;

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
        void (*resize)                (Window* window, int width, int height);
        void (*mouse_press)           (Window* window, int x, int y, unsigned int button);
        void (*mouse_release)         (Window* window, int x, int y, unsigned int button);
        void (*mouse_move)            (Window* window, int x, int y);
        void (*key_press)             (Window* window, unsigned int key);
        void (*key_release)           (Window* window, unsigned int key);
        void (*text_input)            (Window* window, const std::string &text, unsigned int key);
        void (*selection_text_input)  (Window* window, const std::string &text);
        void (*close)                 (Window* window);
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

    virtual void setSelection(const std::string &text) = 0;

    virtual bool hasSelection() = 0;

    virtual void requestSelection() = 0;

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