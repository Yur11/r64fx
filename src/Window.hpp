#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

namespace r64fx{

class MouseEvent;
class KeyEvent;

/** @brief Base class for window implementations. */
class Window{
public:
    enum class Type{
        Normal,
        GL3,
        BestSupported
    } m_Type;
    
private:
    Window();
    
public:
    static Window* createNew(Type type = Type::BestSupported);
    
    virtual ~Window();
    
    void show();
    
    void hide();
    
    void mousePressEvent(MouseEvent* event);
    
    void mouseReleaseEvent(MouseEvent* event);
    
    void mouseMoveEvent(MouseEvent* event);
    
    void keyPressEvent(KeyEvent* event);
    
    void keyReleaseEvent(KeyEvent* event);
    
    void closeEvent();
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H