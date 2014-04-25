#ifndef R64FX_GUI_MENU_H
#define R64FX_GUI_MENU_H

#include "ActionWidget.h"
#include "LinearContainer.h"


namespace r64fx{
    
class Menu : public LinearContainer{
    Font* _font;
    Widget* highlighted_widget = nullptr;
    
    Painter p;
    
public:
    Menu(Font* font = Font::defaultFont(), Widget* parent = nullptr);
    
    void appendAction(Action* act);
    
    void update();
    
    virtual void render();    
};
    
}//namespace r64fx

#endif//R64FX_GUI_MENU_H