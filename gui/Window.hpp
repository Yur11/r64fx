#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include <string>
#include "Rect.hpp"
#include "RenderingContext.hpp"
#include "CallbackList.hpp"

namespace r64fx{

class Widget;
    
/** @brief Base class for window implementations. */
class Window : public RenderingContext{
    typedef void(*VoidFun)(void);
    
    static VoidFun event_callback;

    static Window* currently_rendered_window;
    
    Widget* root_widget = nullptr;
            
    int w;
    int h;
    
    void updateGeometry();
    
    CallbackList one_shot_list;
    
    void runOneShotList();
            
protected:
    inline static void setEventCallback(VoidFun fun) { event_callback = fun; }
                
public:
    Window(RenderingContextId_t id);
    
    virtual ~Window();
    
    virtual void render();
        
    inline void setRootWidget(Widget* widget) { root_widget = widget; }
    inline Widget* rootWidget() const { return root_widget; }
    
    virtual Size<int> size() = 0;
        
    inline int width() { return size().w; }
    
    inline int height() { return size().h; }

    static std::vector<Window*> allInstances();
        
    inline static unsigned int count() { return allInstances().size(); }
    
    /** @brief Tell this window to get destroyed. */
    void discard();
        
    virtual void show() = 0;
    
    virtual void hide() = 0;
    
    virtual void warpMouse(int x, int y) = 0;
    
    virtual bool isShown() = 0;
    
    virtual bool isMaximized() = 0;

    virtual bool isMinimized() = 0;
    
    virtual void turnIntoMenu() = 0;
        
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
        
    static bool initGlew();
    
    static void mainSequence();
    
    inline static Window* currentlyRenderedWindow() { return currently_rendered_window; }
    
    inline void fireOnce(void (*fun)(void*), void* data) { one_shot_list.push_back(fun, data); }
    
    void issueRepaint();
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H