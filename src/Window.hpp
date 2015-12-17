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
        void (*mouse_press)   (Window* window, float x, float y, unsigned int button);
        void (*mouse_release) (Window* window, float x, float y, unsigned int button);
        void (*mouse_move)    (Window* window, float x, float y);
        void (*key_press)     (Window* window, int key);
        void (*key_release)   (Window* window, int key);
        void (*resize)        (Window* window, int old_w, int old_h, int new_w, int new_h);
        void (*close)         (Window* window);
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


    static Window* newInstance(
        int width = 800, int height = 600,
        std::string title = "",
        Window::Type type = Window::Type::Image
    );

    static void deleteInstance(Window* window);

    static void processSomeEvents(Window::Events* events);

    static void forEachWindow(void (*fun)(Window* window, void* data), void* data);
};

};

#endif//R64FX_WINDOW_HPP