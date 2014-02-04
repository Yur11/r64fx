#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include "View.h"
#include "Menu.h"
#include "Event.h"

namespace r64fx{
    
/** @brief Base class for window implementations. */
class Window : public RenderingContext{
    typedef void(*VoidFun)(void);
    
    static VoidFun event_callback;
    
    SplittableView* _view = nullptr;
    
    std::vector<Widget*> _overlay_menus;
    
    Widget* overlay_menu_at(int x, int y);
    
    bool projection_update_needed = false;
    int new_w;
    int new_h;
    
    void update_projection();
    
protected:
    inline static void setEventCallback(VoidFun fun) { event_callback = fun; }
    
    void render_overlay_menus(RenderingContextId_t context_id);
    
    void render(RenderingContextId_t context_id);
    
    inline void request_projection_update(int w, int h) 
    { 
        projection_update_needed = true;
        new_w = w;
        new_h = h;
    }
    
public:
    Window(RenderingContextId_t id);
    
    virtual ~Window();
    
    virtual void render() = 0;
        
    inline void setView(SplittableView* view) { _view = view; }
    inline SplittableView* view() const { return _view; }
    
    virtual Size<int> size() = 0;
    
    virtual void updateGeometry() = 0;
    
    inline int width() { return size().w; }
    
    inline int height() { return size().h; }
    
    void showOverlayMenu(int x, int y, Menu* menu);
    
    void closeOverlayMenu(Menu* menu);
    
    void closeAllOverlayMenus();

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
        
    /** @brief Create and deliver a new mouse press event, taking into acount possible overlays. */
    void initMousePressEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers);
    
    /** @brief Create and deliver a new mouse release event, taking into acount possible overlays. */
    void initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers);
    
    /** @brief Create and deliver a new mouse move event, taking into acount possible overlays. */
    void initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initMouseWheelEvent(int x, int y, int dx, int dy, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initKeyPressEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initKeyReleaseEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers);
    
    void initTextInputEvent(Utf8String text);
        
    static bool initGlew();
    
    static void mainSequence();
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H