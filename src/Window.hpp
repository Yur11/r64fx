#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include "WindowImplIface.hpp"

namespace r64fx{

/** @brief Public window interface. */
class Window : public WindowImplIface{

public:
    enum class Type{
        Normal,
        GL3,
        BestSupported
    };

private:
    Widget* m_root_widget;

    Window::Type m_Type;

    Window(Widget* root);

    virtual ~Window();

public:
    static Window* createNew(Widget* root, Window::Type type = Window::Type::BestSupported);

    static void destroy(Window* window);

    void show();

    void hide();

    void resize(int w, int h);

    void setTitle(const char* title);

    const char* title() const;

    inline Window::Type type() const { return m_Type; }

    inline Widget* rootWidget() const { return m_root_widget; }
};

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H