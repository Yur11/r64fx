#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#include "View.h"
#include "Menu.h"
#include "Event.h"

namespace r64fx{
    
/** @brief Base class for window implementations. */
class Window{
    SplittableView* _view = nullptr;
    
    std::vector<Widget*> _overlay_menus;
    
    Widget* overlay_menu_at(int x, int y);
    
protected:
    void render_overlay_menus();
    
public:
    Window();
    
    virtual ~Window();
    
    void render();
        
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
    
    virtual void makeCurrent() = 0;
    
    virtual void warpMouse(int x, int y) = 0;
    
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
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_H