#ifndef R64FX_GUI_MENU_ITEM_H
#define R64FX_GUI_MENU_ITEM_H

#include "Widget.h"
#include "Action.h"

namespace r64fx{
    
/** @brief A widget to be put inside a menu. */
class MenuItem : public Widget{
    Action* _action;

public:
    MenuItem(Action* action, Widget* parent = nullptr);
    
    virtual Action* action() const { return _action; }
    
    virtual void mousePressEvent(MouseEvent* event);
    
};
    
}//namespace r64fx

#endif//R64FX_GUI_MENU_ITEM_H