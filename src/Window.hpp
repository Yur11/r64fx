#ifndef R64FX_WINDOW_HPP
#define R64FX_WINDOW_HPP

#include <string>

namespace r64fx{

class Widget;
class Image;

class Window{
    Widget* m_Widget = nullptr;

public:
    enum class Type{
        Normal
#ifdef R64FX_USE_GL
        ,GL
#endif//R64FX_USE_GL
    };

    struct Events{
        void (*mouse_press)   (Window* window, float x, float y, unsigned int buttons);
        void (*mouse_release) (Window* window, float x, float y, unsigned int buttons);
        void (*mouse_move)    (Window* window, float x, float y, unsigned int buttons);
        void (*key_press)     (Window* window, int key);
        void (*key_release)   (Window* window, int key);
        void (*resize)        (Window* window, int old_w, int old_h, int new_w, int new_h);
        void (*close)         (Window* window);
    };

private:
    Window::Type m_type;

protected:
    Window(Window::Type type);
    Window(const Window&);

public:
    virtual ~Window();

    static Window* newWindow(
        int width = 800, int height = 600,
        std::string title = "",
        Window::Type type = Window::Type::Normal
    );

    inline void setWidget(Widget* widget) { m_Widget = widget; }

    inline Widget* widget() const { return m_Widget; }

    inline Window::Type type() const { return m_type; }

    void show();

    void hide();

    void resize(int width, int height);

    void repaint();

    Image* image() const;

    void getComponentIndices(int* r, int* g, int* b, int* a);

    void setTitle(std::string title);

    std::string title() const;

    static void processSomeEvents(Window::Events* events);
};

};

#endif//R64FX_WINDOW_HPP