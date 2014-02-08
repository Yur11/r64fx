#ifndef R64FX_GUI_ACTION_WIDGET_H
#define R64FX_GUI_ACTION_WIDGET_H

#include "Action.h"
#include "Font.h"

namespace r64fx{

/** @brief Widget that triggers an action by clicking on it. 
 
    Can be used for buttons and menu items.
 */
struct ActionWidget : public Widget, public Padding{
    Action* action;
    Font* font;
    bool is_highlighted = false;
    bool showing_icon = true;
    
    ActionWidget(Action* act, Font* font, Widget* parent = nullptr);
    
    virtual void render(RenderingContextId_t context_id);
    
    virtual void update();
    
    inline void trigger() { action->trigger(); }
};
    
    
}//namespace r64fx

#endif//R64FX_GUI_ACTION_WIDGET_H