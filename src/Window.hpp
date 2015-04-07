#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include <string>
#include <vector>
#include "Rect.hpp"
#include "Castable.hpp"

namespace r64fx{

class Widget;
    
/** @brief Base class for window implementations. */
class Window : public Castable{
    typedef void(*VoidFun)(void);
    
    static VoidFun event_callback;

    static Window* currently_rendered_window;
    
    Widget* root_widget = nullptr;
            
    int w;
    int h;
    
    void updateGeometry();
    
    void runOneShotList();
            
protected:
    inline static void setEventCallback(VoidFun fun) { event_callback = fun; }
                
public:
    
    Window();
    
    virtual ~Window();
    
    virtual void flush() = 0;
    
    inline void setRootWidget(Widget* widget) { root_widget = widget; }
    inline Widget* rootWidget() const { return root_widget; }
    
    virtual Size<int> size() = 0;
        
    inline int width() { return size().w; }
    
    inline int height() { return size().h; }
    
    virtual void resize(int w, int h) = 0;
    
    inline void resize(Size<int> size) { resize(size.w, size.h); }
    
    inline void setWidth(int w) { resize(w, height()); }
    
    inline void setHeight(int h) { resize(width(), h); }

    static std::vector<Window*> allInstances();
        
    inline static unsigned int count() { return allInstances().size(); }
        
    virtual void show() = 0;
    
    virtual void hide() = 0;
    
    virtual void warpMouse(int x, int y) = 0;
    
    virtual bool isShown() = 0;
    
    virtual bool isMaximized() = 0;

    virtual bool isMinimized() = 0;
    
    virtual void turnIntoMenu() = 0;
    
    virtual bool isSDL2();
           
    /** @brief Create and deliver a new mouse press event, taking into acount possible overlays. */
    void initMousePressEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers);
    
    /** @brief Create and deliver a new mouse release event, taking into acount possible overlays. */
    void initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers);
    
    /** @brief Create and deliver a new mouse move event, taking into acount possible overlays. */
    void initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initMouseWheelEvent(int x, int y, int dx, int dy, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initKeyPressEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initKeyReleaseEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initTextInputEvent(std::string text);
    
    void initResizeEvent(int w, int h);
        
    static void mainSequence();
    
    inline static Window* currentlyRenderedWindow() { return currently_rendered_window; }
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H