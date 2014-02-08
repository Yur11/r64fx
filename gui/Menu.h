#ifndef R64FX_GUI_MENU_H
#define R64FX_GUI_MENU_H

#include "containers.h"
#include "ActionWidget.h"


namespace r64fx{
    
class Menu : public VerticalContainer{
    Font* _font;
    Widget* highlighted_widget = nullptr;
    
public:
    Menu(Font* font, Widget* parent = nullptr);
    
    void appendAction(Action* act);
    
    virtual void render(RenderingContextId_t context_id);
    
    virtual void mousePressEvent(MouseEvent* event);

    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_GUI_MENU_H