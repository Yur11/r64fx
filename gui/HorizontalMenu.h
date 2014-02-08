#ifndef R64FX_GUI_HORIZONTAL_MENU_H
#define R64FX_GUI_HORIZONTAL_MENU_H

#include "Menu.h"

namespace r64fx{
    
/** @brief A horizontal menu bar. */
class HorizontalMenu : public HorizontalContainer{
    Font* _font;
    Window* _last_origin_window;
    
public:
    HorizontalMenu(Font* font, Widget* parent = nullptr);
    
    void appendMenu(Utf8String name, Menu* menu);
    
    virtual void mousePressEvent(MouseEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_GUI_HORIZONTAL_MENU_H