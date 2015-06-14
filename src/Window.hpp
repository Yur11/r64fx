#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

namespace r64fx{

class Widget;
class MouseEvent;
class KeyEvent;

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
    
public:
    static Window* createNew(Widget* root, Type type = Type::BestSupported);
    
    virtual ~Window();
    
    inline void  setImplData(void* data) { m_impl_data = data; }

    inline void* getImplData() const { return m_impl_data; }

    inline Widget* rootWidget() const { return m_root_widget; }
    
    void show();
    
    void hide();
    
    void resize(int w, int h);

    void mousePressEvent(MouseEvent* event);
    
    void mouseReleaseEvent(MouseEvent* event);
    
    void mouseMoveEvent(MouseEvent* event);
    
    void keyPressEvent(KeyEvent* event);
    
    void keyReleaseEvent(KeyEvent* event);
    
    void closeEvent();
};

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H