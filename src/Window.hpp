#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include "WindowImplIface.hpp"
#include "WindowDefs.hpp"
#include "Painter.hpp"

namespace r64fx{

/** @brief Public window interface. */
class Window : public WindowImplIface{
    Widget* m_root_widget = nullptr;
    Painter* m_painter = nullptr;

    Window(Widget* root);

    virtual ~Window();

public:
    static Window* createNew(Widget* root, PainterType pt, WindowType wt, const char* title = nullptr);

    static void destroy(Window* window);

    void show();

    void hide();

    void resize(int w, int h);

    void setTitle(const char* title);

    const char* title();

    inline Widget* rootWidget() const { return m_root_widget; }

    inline Painter* painter() const { return m_painter; }
};

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H