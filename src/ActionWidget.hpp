#ifndef R64FX_GUI_ACTION_WIDGET_H
#define R64FX_GUI_ACTION_WIDGET_H

#include "HoverableWidget.hpp"
#include "Action.hpp"
#include "Font.hpp"
#include "Icon.hpp"
#include "Padding.hpp"

namespace r64fx{

/** @brief Widget that triggers an action by clicking on it. 
 
    Can be used for buttons and menu items.
 */
struct ActionWidget : public HoverableWidget, public Padding<float>{
    Action* action;
    Font* font;
    bool is_highlighted = false;
    bool showing_icon = true;
    
    Icon icon;
    
    ActionWidget(Action* act, Font* font = Font::defaultFont(), Widget* parent = nullptr);
    
    virtual void render();
    
    virtual void update();
    
    inline void trigger() { action->trigger(); }
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseEnterEvent(MouseEvent* event);
};
    
    
}//namespace r64fx

#endif//R64FX_GUI_ACTION_WIDGET_H