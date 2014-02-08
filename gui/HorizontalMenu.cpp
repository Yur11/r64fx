#include "HorizontalMenu.h"
#include "MouseEvent.h"
#include "Window.h"
#include <iostream>

using namespace std;

namespace r64fx{
    
    
struct HorizontalMenuItemWidget : public ActionWidget{
    Menu* menu = nullptr;
    
    HorizontalMenuItemWidget(Menu* menu, Action* act, Font* font, Widget* parent = nullptr)
    : ActionWidget(act, font, parent)
    , menu(menu)
    {
    }
};
    
    
HorizontalMenu::HorizontalMenu(Font* font, Widget* parent)
: HorizontalContainer(parent)
, _font(font)
{
    
}
    
    
void HorizontalMenu::appendMenu(Utf8String name, Menu* menu)
{
    auto aw = new HorizontalMenuItemWidget(menu, new Action(name), _font, this);
}


void HorizontalMenu::mousePressEvent(MouseEvent* event)
{
    auto widget = (HorizontalMenuItemWidget*) childAt(event->position());
    if(widget)
    {
        event->originWindow()->showOverlayMenu(event->original_x(), event->original_y(), widget->menu);
        event->has_been_handled = true;
    }
}
    
}//namespace r64fx