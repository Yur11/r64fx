#ifndef R64FX_GUI_WINDOW_BASE_H
#define R64FX_GUI_WINDOW_BASE_H

#include "View.h"
#include "MouseEvent.h"

namespace r64fx{
    
/** @brief Base class for window implementations. */
class WindowBase{
    SplittableView* _view = nullptr;
    
    /** Menus that show up on top of all the other widgets. Kinda all of them do.
        Ordered in the opening order. 
        This way we can add/remove menus to/from the back of the vector.
        Also this allows us to calculate proper positions for nested menus, starting from the first one,
        by simply iterating over the list.
     */
    std::vector<Widget*> _overlay_menus;
    
    Widget* overlay_menu_at(int x, int y);
    
    /* Remove me */
    ContextMenu* menu;
    
    MouseEvent _last_mouse_event;
    
protected:
    void render_overlays();
    
public:
    WindowBase();
    
    void debug_init(ContextMenu* menu);
    
    inline void setView(SplittableView* view) { _view = view; }
    inline SplittableView* view() const { return _view; }
    
    virtual Size<int> size() = 0;
    
    inline int width() { return size().w; }
    
    inline int height() { return size().h; }
    
    void showOverlayMenu(int x, int y, VerticalMenu* menu);
    
    void closeOverlayMenu(VerticalMenu* menu);
    
    void closeAllOverlayMenus();
    
protected:
    /** @brief Create and deliver a new mouse press event, taking into acount possible overlays. */
    void initMousePressEvent(int x, int y, unsigned int buttons);
    
    /** @brief Create and deliver a new mouse release event, taking into acount possible overlays. */
    void initMouseReleaseEvent(int x, int y, unsigned int buttons);
    
    /** @brief Create and deliver a new mouse move event, taking into acount possible overlays. */
    void initMouseMoveEvent(int x, int y, unsigned int buttons);
    
    void initMouseWheelEvent(int dx, int dy, unsigned int buttons);
    
    void initKeyPressEvent(unsigned int scancode, unsigned int buttons);
    
    void initKeyReleaseEvent(unsigned int scancode, unsigned int buttons);
    
    void initTextInputEvent(Utf8String text);
};    

}//namespace r64fx

#endif//R64FX_GUI_WINDOW_BASE_H