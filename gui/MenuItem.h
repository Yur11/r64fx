#ifndef R64FX_GUI_MENU_ITEM_H
#define R64FX_GUI_MENU_ITEM_H

#include "containers.h"
#include "TextLine.h"
#include "Icon.h"
#include "KeyboardShortcut.h"

namespace r64fx{
    
/** @brief A widget to be clicked on. Can be a menu item or something. */
class MenuItem : public HorizontalContainer{
    
public:
    MenuItem(Icon* icon = nullptr, TextLine* _caption = nullptr, TextLine* _shortcut = nullptr, Widget* parent = nullptr);

    Message on_click;
    
    inline Icon* icon() const { return (Icon*) child(0); }
    
    inline TextLine* caption() const { return (TextLine*) child(1); }
    
    inline TextLine* shortcut() const { return (TextLine*) child(2); }
    
    virtual void mousePressEvent(MouseEvent* event);
    
};
    
}//namespace r64fx

#endif//R64FX_GUI_MENU_ITEM_H