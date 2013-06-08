#ifndef R64FX_GUI_WINDOW_BASE_H
#define R64FX_GUI_WINDOW_BASE_H

#include "View.h"
#include "Menu.h"
#include "MouseEvent.h"

namespace r64fx{
    
/** @brief Base class for window implementations. */
class WindowBase{
    SplittableView* _view = nullptr;
    
    std::vector<Widget*> _overlay_menus;
    
    Widget* overlay_menu_at(int x, int y);
    
    MouseEvent _last_mouse_event;
    
protected:
    void render_overlay_menus();
    
public:
    WindowBase();
    
    inline void setView(SplittableView* view) { _view = view; }
    inline SplittableView* view() const { return _view; }
    
    virtual Size<int> size() = 0;
    
    inline int width() { return size().w; }
    
    inline int height() { return size().h; }
    
    void showOverlayMenu(int x, int y, Menu* menu);
    
    void closeOverlayMenu(Menu* menu);
    
    void closeAllOverlayMenus();
    
protected:
    /** @brief Create and deliver a new mouse press event, taking into acount possible overlays. */
    void initMousePressEvent(int x, int y, unsigned int buttons);
    
    /** @brief Create and deliver a new mouse release event, taking into acount possible overlays. */
    void initMouseReleaseEvent(int x, int y, unsigned int buttons);
    
    /** @brief Create and deliver a new mouse move event, taking into acount possible overlays. */
    void initMouseMoveEvent(int x, int y, unsigned int buttons);
    
    void initMouseWheelEvent(int dx, int dy, unsigned int buttons);
    
    void initKeyPressEvent(unsigned int scancode, unsigned int buttons, unsigned int modifiers);
    
    void initKeyReleaseEvent(unsigned int scancode, unsigned int buttons,  unsigned int modifiers);
    
    void initTextInputEvent(Utf8String text);
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_BASE_H