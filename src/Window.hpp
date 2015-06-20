#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include "Size.hpp"

namespace r64fx{

class Widget;
class MouseEvent;
class KeyEvent;
class ResizeEvent;

/** @brief Base class for window implementations. */
class Window{
    void* m_impl_data = nullptr;

    Widget* m_root_widget;
    
public:
    enum class Type{
        Normal,
        GL3,
        BestSupported
    } m_Type;
    
private:
    Window(Widget* root);
    
    virtual ~Window();
    
public:
    static Window* createNew(Widget* root, Type type = Type::BestSupported);
    
    static void destroy(Window* window);
    
    inline void  setImplData(void* data) { m_impl_data = data; }

    inline void* getImplData() const { return m_impl_data; }

    inline Widget* rootWidget() const { return m_root_widget; }
    
    void show();
    
    void hide();
    
    void resize(int w, int h);
    
    Size<int> size();

    void mousePressEvent(MouseEvent* event);
    
    void mouseReleaseEvent(MouseEvent* event);
    
    void mouseMoveEvent(MouseEvent* event);
    
    void keyPressEvent(KeyEvent* event);
    
    void keyReleaseEvent(KeyEvent* event);
    
    void resizeEvent(ResizeEvent* event);
    
    void closeEvent();
    
    void resizeEvent();
};

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H